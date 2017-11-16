/*
    heard.h -- Structure definitions for nodes heard

  Poor Man's Packet (PMP)
  Copyright (c) 1991 by Andrew C. Payne    All Rights Reserved.

  Permission to use, copy, modify, and distribute this software and its
  documentation without fee for NON-COMMERCIAL AMATEUR RADIO USE ONLY is hereby
  granted, provided that the above copyright notice appear in all copies.
  The author makes no representations about the suitability of this software
  for any purpose.  It is provided "as is" without express or implied warranty.
*/

#include <stdio.h>
#include <time.h>

/* MHEARD structures and defines */


#define MAXID    40     /* max length of broadcast ID */
#define MAXHEARD 15     /* maximum stations in mheard list */

typedef struct mheard {
    struct ax25_addr    call;       /* source address           */
    struct ax25_addr    dest;       /* destination address      */
    time_t              time;       /* time last heard          */
    FILE                *fptr;      /* File pointer             */
    BYTE                s_nr;       /* Sended number            */
    BYTE                r_nr;       /* Received number          */
    long                count;      /* # packets rec'd          */
    long                bytes;      /* # bytes sent             */
    long                flags;      /* node flags               */
    int                 frametype;  /* status of connection     */
    WORD                crc;        /* Received CRC of packet   */
    int                 dup;        /* Duplicate counter        */
    BYTE                id[MAXID];  /* node ID                  */
    char                path[80];   /* Path of station          */
}
MHEARD;

#define HEARD_DIRECT   0x0001   /* heard direct             */
#define HEARD_DIGI     0x0002   /* heard digipeating        */
#define HEARD_IP       0x0004   /* heard w/ TCP/IP          */
#define HEARD_NETROM   0x0008   /* heard w/ NET/ROM         */
#define HEARD_INFO     0x0010   /* heard w/ any info packet */
#define HEARD_CODE     0x0020   /* heard coded data         */
#define HEARD_BIN      0x0040   /* heard #BIN#              */
#define HEARD_ARP      0x0080   /* heard ARP data           */
#define HEARD_MAIL     0x0100   /* Heard 'Mail for:'        */
#define HEARD_FBB      0x0200   /* Heard to FBB broadcast   */

/*-----/ PROTOTYPES /-----*/

void HeardInit(void);
void HeardExit(void);
void Heard(struct ax25_packet *p);
void DoHeard(void);
void log_fbb(AX25_PACKET *p);

#define HEARD_NOT_FOUND  -1
#define HEARD_LIST_EMPTY -2
#define HEARD_LIST_FULL  -3

/*-------------------------------
  FBB
--------------------------------*/
typedef struct fbb {
  int  nr;
  char stat[5];
  int  bid;
  char name[6];
  char bbs[6];
  char src[6];
  char date[6];
  char descr[30];
}
FBB;



