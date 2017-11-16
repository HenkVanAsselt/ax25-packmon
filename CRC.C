/*#+File*********************************************
*
* FILE: CRC.C
* DESC: Calculation of CRC of level 1 packet
* HIST: 940324 V0.1
*
**#-File*********************************************/

/*-----/ Include files /-----*/

#include <stdio.h>
#include <stdlib.h>
#include "packmon.h"
#include "ax25.h"
#include "crctab.h"
#include "crc.h"

/*-#+func----------------------------------------------------------
    FUNCTION: get_CRC()
     PURPOSE: Get CRC of a level 1 packet
      SYNTAX: WORD get_CRC(struct ax25_level1 *packet);
 DESCRIPTION: -
     RETURNS: CRC of packet received (not calculated)
     HISTORY: 940624 V0.1
--#-func----------------------------------------------------------*/
WORD get_CRC(struct ax25_packet *packet)
{
  int  pos;
  WORD *p;

  pos = packet->dlen - 2;
  p = (WORD *) packet->data - pos;
  return(*p);
}

/*-#+func----------------------------------------------------------
    FUNCTION: CRCcheck()
     PURPOSE: Generates a CRC of a level 1 packet, and checks
              this with the existing CRC
      SYNTAX: int CRCCheck(struct ax25_level1 *packet);
 DESCRIPTION: -
     RETURNS: 1 if CRC is valid (equal), 0 if not
     HISTORY: 940324 V0.1
--#-func----------------------------------------------------------*/
int CRCcheck(struct ax25_level1 *packet)
{
    WORD    crc;
    int     len;        /* packet length, data bytes */
    int     i;
    BYTE    *p,t;
    WORD    q;

    assert(packet);

    /*-----/ Intializations /-----*/
    p = packet->data;       /* set pointer to data */
    len = packet->len-2;    /* calculate length less CRC bytes */
    crc = 0xffff;           /* initialize */

    /*-----/ Calculate CRC /-----*/
    for(i=0; i<len; i++) {
        t = *p++ ^ (crc & 0xff);
        crc >>= 8;
        crc ^= crc_table[t];
    }
    q = ~crc;

    crc = *((WORD *)p);     /* fetch existing       */
    /*  *((WORD *)p) = q;   */ /* store computed one   */

    return crc == q;        /* return OK or not OK  */
}




