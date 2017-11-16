/***********************************************
  FILE: NETDUMP.C
  DESC: Netrom dump routines
  DATE: 940311 V0.1
**********************************************/

/*----- INCLUDES -----*/

#include <stdio.h>
#include <memory.h>
#include <string.h>
#include <ctype.h>
#include "buf.h"
#include "packmon.h"
#include "netrom.h"
#include "misc.h"
#include "config.h"
#include "ax25.h"

/*-----/ Defines /-----*/

#define DEBUG

#ifdef DEBUG
#define deb(exp) { \
    fprintf(stdaux, _debugstring, __FILE__, __LINE__, #exp); \
                 }
#else
#define deb(exp)
#endif

/*-----/ LOCAL VARIABLES /-----*/

static char _debugstring[] = "file %s line %d : %s\n\r";

/*-#+func----------------------------------------------------------------
    FUNCTION: NetRomDump()
     PURPOSE: Dump netrom message
      SYNTAX: char *NetRomDump(MBUF *bpp, BYTE flags);
 DESCRIPTION: Given a Net/Rom packet, dumps it in human readable
              form (adds it to ptext)
     RETURNS: pointer to readable text string
     HISTORY: 940723 V0.2
--#-func----------------------------------------------------------------*/
char *NetRomDump(MBUF *bpp, BYTE flags)
{
    char    t[80];
    int i;
    int data;
    static char dtext[1024];
	char src[AXALEN],dest[AXALEN];
	char tmp[AXBUF];
	char thdr[NR4MINHDR];

    /*-----/ Initialize variables /-----*/
    dtext[0] = '\0';
    data = FALSE;

	/*ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
	³ Handle routing broadcasts ³
	ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ*/
	if(uchar(*bpp->data) == NR3NODESIG) {
      debug("NR3NODESIG");
      pullup(bpp,t,1);		/* Signature */
      strcat(dtext,"Routing for ");
      pullup(bpp,t,6);
      t[6] = '\0';
      strcat(dtext,t);
      strcat(dtext,"\n");
      for(i=0; i<11; i++) {
        /*-----/ Destination /-----*/
        if (pullup(bpp,src,AXALEN) < AXALEN) {
          break;
        }
        sprintf(t,"        %12s",GetAX25Addr((struct ax25_addr *) src));
        strcat(dtext,t);
        /*-----/ Alias /-----*/
        pullup(bpp,tmp,ALEN);
        tmp[ALEN] = '\0';
        sprintf(t,"%8s",tmp);
        strcat(dtext,t);
        /*-----/ Best neighbor /-----*/
        pullup(bpp,src,AXALEN);
        sprintf(t,"    %12s",GetAX25Addr((struct ax25_addr *) src));
        strcat(dtext,t);
        /*-----/ Quality /-----*/
        tmp[0] = pullchar(bpp);
        sprintf(t,"    %3u\n",uchar(tmp[0]));
        strcat(dtext,t);
      }
      return(dtext);
    }
    /*ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
    ³ Handle POLL signature     ³
    ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ*/
    else if (uchar(*bpp->data) == NR3POLLSIG) {
      debug("NR3POLLSIG");
      pullchar(bpp);        /* Signature */
      pullup(bpp,tmp,ALEN);
      tmp[ALEN] = '\0';
      sprintf(dtext,"NET/ROM Poll: %s\n",tmp);
      return(dtext);
    }

    /*ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
    ³ Decode network layer      ³
    ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ*/
    /*-----/ Source /-----*/
	pullup(bpp,src,AXALEN);
	sprintf(t,"%s",GetAX25Addr((struct ax25_addr *) src));
    strcat(dtext,t);

    /*-----/ Destination /-----*/
	pullup(bpp,dest,AXALEN);
	sprintf(t,"->%s",GetAX25Addr((struct ax25_addr *) dest));
    strcat(dtext,t);

    /*-----/ ttl /-----*/
	i = pullchar(bpp);
	sprintf(t," ttl %d  ",i);
    strcat(dtext,t);

    /* Read first five bytes of "transport" header */
    pullup(bpp,thdr,NR4MINHDR);
    switch(thdr[4] & NR4OPCODE){

      case NR4OPPID:    /* network PID extension */
        debug("NR4OPPID");
        if (thdr[0] == NRPROTO_IP && thdr[1] == NRPROTO_IP) {
            debug("IP_DUMP() should be called");
            /*
            ip_dump(fp,bpp,check) ;
            */
            return("\0");
        }
        else
            sprintf(t," protocol family %x, proto %x",
             uchar(thdr[0]), uchar(thdr[1])) ;
            strcat(dtext,t);
        break ;

      case NR4OPCONRQ:  /* Connect request */
        debug("NR4OPCONRQ");
        sprintf(t," conn rqst: ckt %d/%d",uchar(thdr[0]),uchar(thdr[1]));
        strcat(dtext,t);
        i = pullchar(bpp);
        sprintf(t," wnd %d",i);
        strcat(dtext,t);
        pullup(bpp,src,AXALEN);
        sprintf(t," %s",GetAX25Addr((struct ax25_addr *) src));
        strcat(dtext,t);
        pullup(bpp,dest,AXALEN);
        sprintf(t,"@%s",GetAX25Addr((struct ax25_addr *) dest));
        strcat(dtext,t);
        break;

      /*-------------------------------
        Connect acknowledgment
      --------------------------------*/
      case NR4OPCONAK:
        debug("NR4OPCONAK");
        sprintf(t," conn ack: ur ckt %d/%d my ckt %d/%d",
         uchar(thdr[0]), uchar(thdr[1]), uchar(thdr[2]),
         uchar(thdr[3]));
        strcat(dtext,t);
        i = pullchar(bpp);
        sprintf(t," wnd %d",i);
        strcat(dtext,t);
        break;

      /*-------------------------------
        Disconnect request
      --------------------------------*/
      case NR4OPDISRQ:
        debug("NR4OPDISRQ");
        sprintf(t," disc: ckt %d/%d",
          uchar(thdr[0]),uchar(thdr[1]));
        strcat(dtext,t);
       break;

      /*-------------------------------
        Disconnect acknowledgement
      --------------------------------*/
      case NR4OPDISAK:
        debug("NR4OPDISAK");
        sprintf(t," disc ack: ckt %d/%d",
         uchar(thdr[0]),uchar(thdr[1]));
        strcat(dtext,t);
        break;

      /*-------------------------------
        Information
      --------------------------------*/
      case NR4OPINFO:
        debug("NR4OPINFO");
        sprintf(t," info: ckt %d/%d",
         uchar(thdr[0]),uchar(thdr[1]));
        strcat(dtext,t);
        sprintf(t," txseq %d rxseq %d",
         uchar(thdr[2]), uchar(thdr[3]));
        strcat(dtext,t);
        strcat(dtext,"\n");

        if (bpp->cnt > 5) {
          for (i=0 ; i<5 ; i++) {
            if (!isprint(bpp->dptr[i]))
              break;
          }
          if (i==5)
            strcat(dtext,DumpText(bpp->dptr,bpp->cnt));
          else {
            sprintf(t,"[Binary data, len=%3d]\n",bpp->cnt);
            strcat(dtext,t);
          }
        }
        break;

        /*
        if (hex_debug)
          if (bpp->cnt > 0)
            dump_hex((unsigned char *)bpp->dptr,bpp->cnt);
        break;
        */

      /*-------------------------------
        Information acknowledgement
      --------------------------------*/
      case NR4OPACK:
        debug("NR4OPACK");
        sprintf(t," info ack: ckt %d/%d",
         uchar(thdr[0]),uchar(thdr[1]));
        strcat(dtext,t);
        sprintf(t," txseq %d rxseq %d",
         uchar(thdr[2]), uchar(thdr[3]));
        strcat(dtext,t);
        break;

      /*-------------------------------
        Unknown transport type
      --------------------------------*/
      default:
        debug("Unknown transport type");
        sprintf(t," unknown transport type %d",
         thdr[4] & 0x0f) ;
        strcat(dtext,t);
        break;
    }

    if(thdr[4] & NR4CHOKE) {
        sprintf(t," CHOKE");
        strcat(dtext,t);
    }
    if(thdr[4] & NR4NAK)   {
        sprintf(t," NAK");
        strcat(dtext,t);
    }
    if(thdr[4] & NR4MORE)  {
        sprintf(t," MORE");
        strcat(dtext,t);
    }
    strcat(dtext,"\n");
    return(dtext);
}

