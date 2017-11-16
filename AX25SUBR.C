/*
    ax25subr.c -- General AX.25 subroutines

  Poor Man's Packet (PMP)
  Copyright (c) 1991 by Andrew C. Payne    All Rights Reserved.

  Permission to use, copy, modify, and distribute this software and its
  documentation without fee for NON-COMMERCIAL AMATEUR RADIO USE ONLY is hereby
  granted, provided that the above copyright notice appear in all copies.
  The author makes no representations about the suitability of this software
  for any purpose.  It is provided "as is" without express or implied warranty.

    Andrew C. Payne
*/
/* ----- Includes ------ */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <malloc.h>
#include <bios.h>
#include <memory.h>
#include <ctype.h>

#include <mshell.h>

#include "packmon.h"
#include "ax25.h"


char Ax25multi[][AXALEN] = {
	'Q'<<1, 'S'<<1, 'T'<<1, ' '<<1, ' '<<1, ' '<<1, '0'<<1, /* QST */
    'N'<<1, 'O'<<1, 'D'<<1, 'E'<<1, 'S'<<1, ' '<<1, '0'<<1, /* NODES */
	'M'<<1, 'A'<<1, 'I'<<1, 'L'<<1, ' '<<1, ' '<<1, '0'<<1, /* MAIL */
	'I'<<1, 'D'<<1, ' '<<1, ' '<<1, ' '<<1, ' '<<1, '0'<<1, /* ID */
    'O'<<1, 'P'<<1, 'E'<<1, 'N'<<1, ' '<<1, ' '<<1, '0'<<1, /* OPEN */
	'C'<<1, 'Q'<<1, ' '<<1, ' '<<1, ' '<<1, ' '<<1, '0'<<1, /* CQ */
	'B'<<1, 'E'<<1, 'A'<<1, 'C'<<1, 'O'<<1, 'N'<<1, '0'<<1, /* BEACON */
	'R'<<1, 'M'<<1, 'N'<<1, 'C'<<1, ' '<<1, ' '<<1, '0'<<1, /* RMNC */
	'A'<<1, 'L'<<1, 'L'<<1, ' '<<1, ' '<<1, ' '<<1, '0'<<1, /* ALL */
    '\0',
};

/*-#+func----------------------------------------------------------
    FUNCTION: is_ax25multi()
     PURPOSE: Check if given address is a AX25 multi address
      SYNTAX: int is_ax25multi(struct ax25_addr addr);
 DESCRIPTION:
     RETURNS: Index in multi address list if found
              -1 if not found
     HISTORY: 940405 V0.1
--#-func----------------------------------------------------------*/
int is_ax25multi(struct ax25_addr addr)
{
  int i;

  for (i=0 ; i<MAX_MULTI_ADDR ; i++) {
    if (!memcmp(Ax25multi[i],&addr,7))
      return(i);
  }
  return(MA_NONE);           /* Not found */
}

/*-#+func----------------------------------------------------------------
    FUNCTION: eol_in()
     PURPOSE: Given a EOL convention code, a string and a length,
              converts a string with the appropriate EOL convention
              to a '\n' terminated string.
      SYNTAX: void eol_in(int conv, char *s, int l);
 DESCRIPTION:
     RETURNS:
     HISTORY:
--#-func----------------------------------------------------------------*/
void eol_in(int conv, char *s, int l)
{
  switch(conv) {
    case EOL_CR:
      while(l--) {
        if(*s == '\r')
          *s = '\n';
        s++;
      }
    case EOL_LF:
    default:      /* no conversion */
      return;
  }
}


/*--#+func----------------------------------------------------------------
    FUNCTION: FrameType()
     PURPOSE: Given the control byte, returns the type of the frame.
      SYNTAX: int FrameType(BYTE c);
 DESCRIPTION:
     RETURNS:
     HISTORY:
    SEE ALSO:
--#-func---------------------------------------------------------------*/
int FrameType(BYTE c)
{                            /* 765  4  321  0     */

    if(!(c & 1))             /* n(r) p n(s)  0     */
        return I;            /* = Information      */

    if(c & 2)                /* mmm  p/f ss0 1     */
        return c & ~PF;      /* U frames, strip PF */

    else                     /* n(r) p/f mm1 1     */
        return c & 0xf;      /* = S frames         */
}

/*-#+func----------------------------------------------------------------
    FUNCTION: GetAX25Addr()
     PURPOSE:
      SYNTAX: char *GetAX25Addr(struct ax25_addr *a);
 DESCRIPTION: Given a pointer to an ax25_addr record,
              returns an ASCII string of the address in human readable
              form:  'N8KEI-3'
     RETURNS:
     HISTORY:
--#-func--------------------------------------------------------------- */
char *GetAX25Addr(struct ax25_addr *a)
{
    static  char    s[MAXCLEN+5];
    char    *p;
    char    c;
    int i;

    /*-------------------
      translate callsign
    --------------------*/
    p = s;
    for(i=0; i<MAXCLEN; i++) {
        c = (a->call[i] >> 1) & 0x7f;
        if(c == ' ')
            break;
        else
            *p++ = c;
    }

    /*----------------------
      copy ssid if non-zero
    -----------------------*/
    i = (a->ssid >> 1) & 0x0f;
    if(i)
        sprintf(p,"-%d",i);
    else
        *p = '\0';

    /*-------------------------
      return pointer to string
    --------------------------*/
    return s;
}

/*-#+func----------------------------------------------------------
    FUNCTION: CompAX25Addr()
     PURPOSE: Compares two AX.25 addresses.  .
      SYNTAX: int CompAX25Addr(struct ax25_addr *a1, struct ax25_addr *a2)
 DESCRIPTION:
     RETURNS: TRUE if the two AX.25 addresses are NOT equal
     HISTORY:
--#-func----------------------------------------------------------*/
int CompAX25Addr(struct ax25_addr *a1, struct ax25_addr *a2)
{
    /* compare the calls */
    if(memcmp(a1->call,a2->call,sizeof(struct ax25_addr)-1))
        return TRUE;

    /* compare the SSIDs */
    return (a1->ssid & 0x1e) != (a2->ssid & 0x1e);
}

#ifdef DONT_SKIP_THIS

/*-#+func----------------------------------------------------------
    FUNCTION: GetAX25Path()
     PURPOSE: Get path of a given packet
      SYNTAX: char *GetAX25Path(struct ax25_packet *p)
 DESCRIPTION: Given a pointer to a level 2 packet, parses, and returns
              a pointer to a human readable string in the form:
              "N8KEI [via WB2EMS]"
     RETURNS: Pointer to decoded path
     HISTORY: 940722 V0.1
--#-func----------------------------------------------------------*/
/* GetAX25Path(*packet)
*/
char *GetAX25Path(struct ax25_packet *p)
{
    static char s[256];
    int i;
    struct ax25_addr    *ad;

/* copy in the destination call */
    strcpy(s,GetAX25Addr(&p->dest));

/* copy in the digi path */
    if(i = p->ndigis) {
        strcat(s," [via ");
        strcat(s,GetAX25Addr(p->digis));
        ad = p->digis + 1;
        while(i > 1) {
            strcat(s,",");
            strcat(s,GetAX25Addr(ad++));
            i--;
        }
        strcat(s,"]");
    }

    return s;
}

#endif

