/**#+FILE**********************************************************************

  FILE: AX25DUMP.C
  DESC: Dumps AX.25 packets in human readable format
  HIST: 940325 V0.2

******************************************************************************/

/*-----/ Include files /-----*/

#include <stdio.h>
#include <conio.h>
#include <memory.h>
#include <malloc.h>
#include <string.h>
#include <dos.h>
#include <ctype.h>

#include "packmon.h"
#include "misc.h"
#include "buf.h"
#include "arp.h"
#include "ip.h"
#include "ax25.h"
#include "netrom.h"
#include "config.h"

/*-----/ Static variables /-----*/

static MBUF buffer;

#define DEBUG

static char _debugstring[] = "file %s line %d : %s\n\r";

#ifdef DEBUG
#define deb(exp) { \
    fprintf(stdaux, _debugstring, __FILE__, __LINE__, #exp); \
                 }
#else
#define deb(exp)
#endif


/*-#+func----------------------------------------------------------
    FUNCTION: dump_digis()
     PURPOSE: Dump digipeaters
      SYNTAX: char *dump_digis(struct ax25_packet *p);
 DESCRIPTION: Dumps digipeater of packet in a string
     RETURNS: Pointer to string with digipeaters
     HISTORY: 940408 v0.1
--#-func----------------------------------------------------------*/
char *dump_digis(struct ax25_packet *p)
{
  static char dtext[80];
  int i;


  dtext[0] = '\0';      /* Clear string */

  if(p->ndigis) {
    strcat(dtext," [via ");
    for(i=0; i<p->ndigis; i++) {
      strcat(dtext,GetAX25Addr(p->digis + i));
      if(p->repeated[i])
        strcat(dtext,"*");
      if(i != (p->ndigis - 1))
        strcat(dtext,",");
    }
    strcat(dtext,"]");
  }
  return(dtext);
}

/*-#+func----------------------------------------------------------------
    FUNCTION: DumpAX25hdr()
     PURPOSE: Given a pointer to a Level 2 packet structure, creates a
              human readable level 2 header and info field in ptext.
      SYNTAX: char *DumpAX25hdr(struct ax25_packet *p)
 DESCRIPTION: struct ax25_packet *p = pointer to AX25 packet
     RETURNS: pointer to local static string with header dump
     HISTORY: 940311 V0.1 - Initial version
--#-func----------------------------------------------------------------*/
char *DumpAX25hdr(struct ax25_packet *p)
{
    int  i;
    int  showinfo;              /* flag for fields with info */
    int  type;                  /* frame type */
    char t[20];                 /* temp space */
    static char dtext[1024];     /* Space to dump text to */

    dtext[0] = '\0';  /* 'Clear' dumped text array */

    /*--------------------------
      print the to/from address
    ---------------------------*/
    strcat(dtext,GetAX25Addr(&p->source));
    strcat(dtext,">");
    strcat(dtext,GetAX25Addr(&p->dest));

    /*-------------------
      print digipeaters
    --------------------*/
    if(p->ndigis) {
      strcat(dtext," [via ");
      for(i=0; i<p->ndigis; i++) {
        strcat(dtext,GetAX25Addr(p->digis + i));
        if(p->repeated[i])
          strcat(dtext,"*");
        if(i != (p->ndigis - 1))
          strcat(dtext,",");
      }
      strcat(dtext,"]");
    }

    /*-------------------------------
       decode and show control byte
     ------------------------------*/
    strcat(dtext,"  <");
    showinfo = FALSE;
    switch(type = FrameType(p->cont)) {
      case I:     strcat(dtext,"I");        showinfo = TRUE;     break;
      case RR:    strcat(dtext,"RR");                            break;
      case RNR:   strcat(dtext,"RNR");                           break;
      case REJ:   strcat(dtext,"REJ");                           break;
      case SABM:  strcat(dtext,"SABM");                          break;
      case DISC:  strcat(dtext,"DISC");                          break;       /* DISConnect */
      case DM:    strcat(dtext,"DM");                            break;       /* Disconnect Mode */
      case UA:    strcat(dtext,"UA");                            break;
      case FRMR:  strcat(dtext,"FRMR");                          break;
      case UI:    strcat(dtext,"UI");       showinfo = TRUE;     break;
    }

    /*------------------------
       show the protocol ID
    ------------------------*/
    if(showinfo) {
      switch(p->pid) {

        case PID_X25:           /* CCITT X.25 PLP */
          strcat(dtext," (X.25 PLP)");
          break;

        case PID_SEGMENT:       /* Segmentation fragment */
          strcat(dtext," (Segment)");
          break;

        case PID_TEXNET:        /* TEXNET datagram protocol */
          strcat(dtext," (TEXNET)");
          break;

        case PID_LQ:            /* Link quality protocol */
          strcat(dtext," Link Quality)");
          break;

        case PID_APPLETALK:     /* Appletalk */
          strcat(dtext," (APPLETALK)");
          break;

        case PID_APPLEARP:      /* Appletalk ARP */
          strcat(dtext," (APPLEARP)");
          break;

        case PID_IP:            /* ARPA Internet Protocol */
          strcat(dtext," (IP)");
          break;

        case PID_ARP:           /* ARPA Address Resolution Protocol */
          strcat(dtext," (ARP)");
          break;

        case PID_RARP:          /* ARPA Reverse Address Resolution Protocol */
          strcat(dtext," (RARP)");
          break;

        case PID_NETROM:        /* NET/ROM */
          strcat(dtext," (NET/ROM)");
          break;

        case PID_NO_L3:                     /* No level 3 protocol */
          strcat(dtext," (Text)");
          break;

        default:
          sprintf(t," (PID=0x%X)",p->pid);
          strcat(dtext,t);
          break;
      }
    }

    /*----------------------
      show poll/final bit
    -----------------------*/
    if(p->cont & PF) {
      switch(p->cmdresp) {
        case COMMAND:   strcat(dtext," (P)");   break;  /* cmd, poll */
        case RESPONSE:  strcat(dtext," (F)");   break;  /* resp, final */
        case UNKNOWN:   strcat(dtext," (P/F)"); break;
      }
    }

    /*-----------------------
       show sequence numbers
    ------------------------*/
    if((type & 3) != U) {
        sprintf(t," R%d",(p->cont >> 5) & 7);
        strcat(dtext,t);
    }
    if(type == I) {
        sprintf(t," S%d",(p->cont >> 1) & 7);
        strcat(dtext,t);
    }
    strcat(dtext,">");

    return(dtext);          /* Return pointer to header dump */
}

/*-#+func----------------------------------------------------------
    FUNCTION: DumpText()
     PURPOSE: Dump text from packet with PID_TEXT
      SYNTAX: char *DumpText(BYTE *d, unsigned int len);
 DESCRIPTION:
     RETURNS: Pointer to text string
     HISTORY:
--#-func----------------------------------------------------------*/
char *DumpText(BYTE *d, unsigned int len)
{
  static char dtext[1024];
  int i,j;


  /*-----/ Initialize string and variables /-----*/
  dtext[0] = '\0';
  i = 0;
  j = len;

  /*-----/ Dump text to string /-----*/
  while (j > 0) {
    if (*d >= 0x20)
      dtext[i++] = *d++;
    else
      *d++;
    j--;
    if (*d == 0x0D)
      dtext[i++] = 0x0A;
  }

  /*-----/ Terminate line and return /-----*/
  dtext[i] = '\0';
  return(dtext);
}

/*-#+func----------------------------------------------------------------
    FUNCTION: DumpAX25()
     PURPOSE: Convert ax25 packet to human readable string
      SYNTAX: char *DumpAX25(struct ax25_packet *p);
 DESCRIPTION:
     RETURNS: pointer to string containing converted packet
     HISTORY: 940212 V0.1
              940312 V0.2 - Now return pointer to string
--#-func----------------------------------------------------------------*/
char *DumpAX25(struct ax25_packet *p, BYTE flags)
{
    static char dtext[1024];
    char t[80];
    long curtime;

    dtext[0] = '\0';

/*
    if (hex_debug)
      if (p->dlen > 0)
        dump_hex((unsigned char *)p->data,p->dlen);
*/

    if (flags & DUMP_HEADER) {
      /*-----/ Show time /-----*/
      curtime = time(NULL);
      sprintf(t,"[%s] ",timestr(curtime));
      strcat(dtext,t);
      /*-----/ Show header /-----*/
      strcpy(dtext,DumpAX25hdr(p));
      if(p->dlen)
        strcat(dtext,":\n");
      else
        strcat(dtext,"\n");
    }

    /*---------------------------
      show the data field if any
    ----------------------------*/
    if (flags & DUMP_INFO) {
      if(p->dlen) {
        switch(p->pid) {
          case PID_NETROM:
            buffer.len = p->dlen;
            memcpy(buffer.data,p->data,p->dlen);
            setbufptr(&buffer,0L);
            strcat(dtext,NetRomDump(&buffer,DUMP_HEADER|DUMP_INFO));
            break;
          case PID_TEXT:
            strcat(dtext,DumpText(p->data,p->dlen));
            break;
          case PID_ARP:
            debug("Dump ARP");
            buffer.len = p->dlen;
            memcpy(buffer.data,p->data,p->dlen);
            setbufptr(&buffer,0L);         /* Intialize buffer pointer */
            strcat(dtext,arp_dump(&buffer,DUMP_HEADER|DUMP_INFO));
            break;
          case PID_IP:
            debug("Dump IP");
             strcat(dtext,ip_dump(p->data,p->dlen));
             break;
          default:
            sprintf(t,"Unknown PID %X",p->pid);
            strcat(dtext,t);
            /*
            strcat(dtext,t);
            dump_hex(p->data, p->dlen);
            */
            break;
        }
      }
    }

    return(dtext);
}
