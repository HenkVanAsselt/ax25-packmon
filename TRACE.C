/*
    TRACE -- Packet tracing routines (for debugging)

  Poor Man's Packet (PMP)
  Copyright (c) 1991 by Andrew C. Payne    All Rights Reserved.

  Permission to use, copy, modify, and distribute this software and its
  documentation without fee for NON-COMMERCIAL AMATEUR RADIO USE ONLY is hereby
  granted, provided that the above copyright notice appear in all copies.
  The author makes no representations about the suitability of this software
  for any purpose.  It is provided "as is" without express or implied warranty.

    August, 1989

    Andrew C. Payne
*/

/* ----- Includes ----- */
#include <stdio.h>
#include <malloc.h>
#include <memory.h>
#include <string.h>
#include <dos.h>
#include <bios.h>
#include <time.h>
#include "packmon.h"

#ifdef TRACE
#define HEX

#define LOGSIZE 1000            /* number of packets in log */

static long clock_count = 0L;

/* ----- Counters ----- */
static long	RXCount;		/* number of good frames received */
static long	RXQOverflow;		/* number of RX Queue overflows */
static long	RXBOverflow;		/* number of RX Buffer overflows */
static long	RXFrameErr;		/* number of framing errors received */
static long	RXCRCErr;		/* number of CRC error received */
static long	RXFRMR;			/* number of FRMR frames received */
static long	RXREJ;			/* number of REJ frames received */
static long	TXCount;		/* number of frames transmitted */
static long	StartTime;		/* startup time */



extern char *DumpLevel2(struct ax25_packet *p);
extern struct ax25_packet *AX25L1toL2(struct ax25_level1 *p);

/* ------ Local Structures ----- */
struct packet_log {
    long    time;                   /* time sent */
    int     dir;                    /* direction */
    struct  packet_log   *next;     /* next packet in linked list */
    struct  ax25_level1 p;          /* the level 1 packet */
};

static struct packet_log **log;
    int nlogs;          /* number of items in log */

static FILE *outfile;       /* output file */

/*-#+func----------------------------------------------------------
    FUNCTION: LogInit()
     PURPOSE: Initialize the logging system
      SYNTAX: void LogInit(void);
 DESCRIPTION:
     RETURNS: nothing
     HISTORY: 940222 V0.1
------------------------------------------------------------------*/
void LogInit()
{
    log = malloc(sizeof(struct packet_log *) * LOGSIZE);
    nlogs = 0;
}

/*-#+func----------------------------------------------------------
    FUNCTION: LogPacket
     PURPOSE:
      SYNTAX: void LogPacket(struct ax25_level1 *p, int dir);
 DESCRIPTION: Given a pointer to a level 1 packet, allocates a record
              for the packet and stores the packet,
              direction (incoming or outgoing), and the time.
     RETURNS: nothing
     HISTORY: 940222 V0.1
------------------------------------------------------------------*/

void LogPacket(struct ax25_level1 *p, int dir)
{
    struct packet_log   *pl;

    /* log full? */
    if(nlogs >= LOGSIZE)
        return;

    /* allocate a structure */
    pl = malloc(sizeof(struct packet_log) + sizeof(struct ax25_level1) + p->len);
    if(pl == NULL) {
        fprintf(stderr,"--- Out of memory in LogPacket\n");
        return;
    }

    /* set up the log packet */
    pl->time = _bios_timeofday(_TIME_GETCLOCK,&clock_count);
    pl->dir = dir;
    memcpy(&pl->p,p,sizeof(struct ax25_level1) + p->len);

    /* insert into log */
    log[nlogs++] = pl;
}

/*-#+func----------------------------------------------------------
    FUNCTION: DumpStatus()
     PURPOSE: Dumps the current PMP status variables to the log file.
      SYNTAX: void DumpStatus(void);
 DESCRIPTION:
     RETURNS:
     HISTORY:
------------------------------------------------------------------*/
static void DumpStatus(void)
{
    long    t;
    struct tm   *tm;

    time(&t);
    tm = localtime(&t);
    fprintf(outfile,"\n----------\n");
    fprintf(outfile,"PMP Status at %02d/%02d/%02d %02d:%02d:%02d\n",
        tm->tm_mon+1, tm->tm_mday, tm->tm_year, tm->tm_hour,
        tm->tm_min, tm->tm_sec);
    fprintf(outfile,"      %8ld good frames received\n",RXCount);
    fprintf(outfile,"      %8ld framing errors\n",RXFrameErr);
    fprintf(outfile,"      %8ld checksum errors\n",RXCRCErr);
    fprintf(outfile,"      %8ld receive queue overflows\n",RXQOverflow);
    fprintf(outfile,"      %8ld receive buffer overflows\n",RXBOverflow);
    fprintf(outfile,"      %8ld REJ frames received\n",RXREJ);
    fprintf(outfile,"      %8ld FRMR frames received\n",RXFRMR);
    fprintf(outfile,"      %8ld frames transmitted\n",TXCount);
    fprintf(outfile,"      %8ld frames in trace log\n",(long)nlogs);
/*  fprintf(outfile,"  %12ld bytes free\n\n",coreleft()); */
}

/*-#+func----------------------------------------------------------
    FUNCTION: DumpEntry()
     PURPOSE: Dumps a log entry to the output file
      SYNTAX: void DumpEntry(struct packet_log *pl);
 DESCRIPTION:
     RETURNS:
     HISTORY:
------------------------------------------------------------------*/
static void DumpEntry(struct packet_log *pl)
{
    struct ax25_packet  *p;
    static long lasttime = 0;
    long    t;

#ifdef HEX
    int i;
    for(i=0; i<pl->p.len; i++)
        fprintf(outfile,"%X ",pl->p.data[i]);
    fprintf(outfile,"\n");
/* #else */
    putchar('.');
    p = AX25L1toL2(&pl->p);
    putchar('-');
    t = pl->time - lasttime;
    lasttime = pl->time;
    fprintf(outfile,"[+%ld]  %s\n",t,DumpLevel2(p));
#endif
    free(pl);
}

/*-#+func----------------------------------------------------------
    FUNCTION: DumpLog()
     PURPOSE: Dumps all of the log entries to logfile
      SYNTAX: void DumpLog(void);
 DESCRIPTION:
     RETURNS:
     HISTORY:
------------------------------------------------------------------*/
void DumpLog(void)
{
    char    fname[80];
    int i;

    if(nlogs) {
      /*
      printf("\n\nFilename for trace log (RETURN for no logfile) --> ");
      gets(fname);
      */
      strcpy(fname,"packmon.log");
      if(*fname) {
        if((outfile = fopen(fname,"w")) == NULL) {
          perror("Can't open output file");
          return;
        }
        fprintf(outfile,"Version:  Packmon %s compiled %s\n\n",VERSION,__DATE__);
        for(i = 0; i<nlogs; i++)
          DumpEntry(log[i]);
        DumpStatus();
        printf("\n%d packets written to logfile %s.\n",nlogs,strupr(fname));
        fclose(outfile);
      }
    }
}

#endif  /* TRACE */

