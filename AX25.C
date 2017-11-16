/***************************************************************
    FILE: AX25.C
    DESC: Handle incoming AX25 packets
    DATE: 940311

***************************************************************/

#define DEBUG

/* ----- Includes ----- */
#include <stdio.h>
#include <stdlib.h>

#include <conio.h>
#include <memory.h>
#include <malloc.h>
#include <string.h>

#ifdef MEM_DEBUG
#include <mshell.h>
#endif

#include "config.h"
#include "packmon.h"

#include "misc.h"
#include "buf.h"
#include "ax25.h"
#include "arp.h"
#include "crc.h"
#include "log.h"
#include "heard.h"
#include "status.h"
#include "netrom.h"

/*---------------------
  Local variables
---------------------*/
static MBUF p1buffer;

/*--#+func----------------------------------------------------------------
    FUNCTION: AX25Level2()
     PURPOSE: This routine is the level 2 upcall
      SYNTAX: void AX25Level2(AX25_LEVEL1 *p)
 DESCRIPTION: Called once for each received level1 packet
              This routine handles logging packets in the debugging log,
              dumping packets packets on screen,
              updating the heard stations list.
     RETURNS: Nothing
     HISTORY: 940223 V0.2
    SEE ALSO: Heard LogPacket
--#-func----------------------------------------------------------------*/
void AX25Level2(AX25_LEVEL1 *p)
{
  AX25_PACKET *p2 = NULL;

  if((p2 = ax25rec(p)) == NULL)     /* Convert AX.25 Level 1 to Level 2  */
      return;                       /* bad packet */

  if (CRCcheck(p))  {
    RXCount++;                      /* Increment good frames counter */
    wn_printf(w_mon,"%s\n",DumpAX25(p2,DUMP_HEADER|DUMP_INFO));
    LogPacket(p);                   /* log packets    */
    Heard(p2);                      /* log nodes heard */
  }
  else {
    RXCRCErr++;                     /* Increment CRC error counter */
    /*
    wn_printf(w_mon,"%s\n",DumpAX25(p2,DUMP_HEADER|DUMP_INFO));
    */
    wn_printf(w_mon,"packet CRC failed\n");
  }

  /*-----/ Show heard list and status /-----*/
  DoHeard();
  DumpStatus();


  /*-----/ Free memory, allocated by level2 packet  /-----*/
  free(p2);
  /* p2 = NULL; /*       /* Not neccessary, but useful for later */
}

/*-#+func----------------------------------------------------------------
    FUNCTION: ax25rec()
     PURPOSE: Level 1 to Level 2 conversion
      SYNTAX: AX25_PACKET *ax25rec(AX25_LEVEL1 *p1)
 DESCRIPTION: Given a pointer to a level 1 packet,
              converts it to a level 2 packet structure and returns
              a pointer to the newly allocated level 2 packetstructure.
     RETURNS: Returns NULL if an error in the structure of
              the level 1 packet
     HISTORY:
--#-func----------------------------------------------------------------*/
AX25_PACKET *ax25rec(AX25_LEVEL1 *p1)
{
    AX25_PACKET p2;
    AX25_PACKET *p;
    BYTE   t1,t2;
    int    cr[4];

    cr[0] = UNKNOWN;
    cr[1] = RESPONSE;
    cr[2] = COMMAND;
    cr[4] = UNKNOWN;

    if(p1->len < 15) {        /* minimum number of octets is 17! */
        RXCRCErr++;                     /* Increment error counter */
        /*
        wn_printf(w_mon,"*** INVALID LENGTH %d ***\n",p1->len);
        */
        return NULL;
    }

    p1buffer.len = p1->len;
    memcpy(p1buffer.data,p1->data,p1buffer.len);
    setbufptr(&p1buffer,0L);         /* Intialize buffer pointer */

    /*-----------------------------------
      copy destination callsign and ssid
    ------------------------------------*/
    buf2data((unsigned char *)&p2.dest,&p1buffer,MAXCLEN);
    t1 = pull8(&p1buffer);
    p2.dest.ssid = t1 & SSIDMASK;

    /*-----------------------------------
      copy source callsign and ssid
    ------------------------------------*/
    buf2data((unsigned char *)&p2.source,&p1buffer,MAXCLEN);
    t2 = pull8(&p1buffer);
    p2.source.ssid = t2 & SSIDMASK;

    /*----------------------
      command/response bits
    -----------------------*/
    p2.cmdresp = (BYTE) cr[(((t1 & REPEATED) != 0) << 1) + ((t2 & REPEATED) != 0)];

    /*-------------------------
      copy digipeaters, if any
    --------------------------*/
    p2.ndigis = 0;
    while(p2.ndigis < MAXDIGIS && ((t2 & 1) == 0)) {
      buf2data((unsigned char *)&p2.digis[p2.ndigis],&p1buffer,MAXCLEN);
      t2 = pull8(&p1buffer);
      p2.digis[p2.ndigis].ssid = t2 & SSIDMASK;
      p2.repeated[p2.ndigis++] = t2 & REPEATED;
    }

    if(p2.ndigis == MAXDIGIS) {      /* too many digipeaters */
      RXCRCErr++;                     /* Increment error counter */
      /*
      wn_printf(w_mon,"*** TO MANY DIGIPEATERS ***\n");
      */
      return NULL;
    }

    /*-----------------
       control field
    -------------------*/
    p2.cont = pull8(&p1buffer);
    switch(FrameType(p2.cont)) {
      case I :
      case UI: p2.pid = pull8(&p1buffer);   /* protocol ID */
               break;
      default: break;

    }

    /*-----------------------------------------------------------
      allocate record and copy in header and data field, if any.
      - 2 bytes for CRC checksum field !
    -----------------------------------------------------------*/
    p2.dlen = p1buffer.cnt - 2;     /* length remaining */

    if(p2.dlen > 256 || p2.dlen < 0) {
      RXCRCErr++;                     /* Increment error counter */
      wn_printf(w_mon,"*** <%d> OUT OF RANGE PACKET ***\n",p2.dlen);
      return NULL;            /* out of range packets */
    }

    /*---------------------------------------
      Allocate memory for AX25 packet + data
    ----------------------------------------*/
    p = malloc(sizeof(AX25_PACKET) + p2.dlen);
    if(p == NULL)
      return NULL;            /* out of memory */

    /*----- Copy packet header -----*/
    memcpy(p, &p2, sizeof(AX25_PACKET));

    /*-----  Copy packet data -----*/
    if(p2.dlen)
      buf2data(p->data,&p1buffer,p2.dlen);

    /*-----  return pointer to allocated packet -----*/
    return p;
}
