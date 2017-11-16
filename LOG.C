/***#+FILE********************************************************************

    FILE: LOG.C
    DESC: Loggin Routines for HvA Packet Monitor
    HIST: 940328 V0.1

****#-FILE********************************************************************/

/* ----- Includes ----- */
#include <stdio.h>
#include <malloc.h>
#include <memory.h>
#include <string.h>

#include <dos.h>
#include <bios.h>
#include <time.h>
#include "packmon.h"
#include "config.h"
#include "ax25.h"
#include "misc.h"

#include <mshell.h>

#define LOG_C
#include "log.h"

/* #define HEX    */            /* Hexadecimal dump in logfile */


/* ----- Counters ----- */
long RXCount;                   /* number of good frames received */
long RXCRCErr;                  /* number of CRC error received */

static FILE *logfile = NULL;            /* pointer to log file */
static char *logname = "packmon.log";   /* Name of this logfile   */

/*-#+func----------------------------------------------------------
    FUNCTION: init_station()
     PURPOSE: Initialize variables of a station
      SYNTAX: init_station(STATION *station);
 DESCRIPTION:
     RETURNS: Nothing
     HISTORY: 940417 V0.1
    SEE ALSO: STATION
--#-func----------------------------------------------------------*/
void init_station(STATION *station)
{
  station->fptr = NULL;
  station->bytes = 0L;
  station->count = 0L;
  station->flags = 0L;
  station->id[0] = '\0';
  station->s_nr  = 0xF;    /* Initialize sended number to number out of range 0..7 */
  station->path[0] = '\0';

}

/*#+func-----------------------------------------------------------------------
    FUNCTION: find_station()
     PURPOSE: Find station call in sorted single linked list
      SYNTAX: STATION *find_station(ax25_addr call);
 DESCRIPTION: -
     RETURNS: Pointer structure in list if found, else returns NULL
     HISTORY: 940415 V0.1
    SEE ALSO: STATION
--#-func----------------------------------------------------------------------*/
STATION *find_station(struct ax25_addr *call)
{
  STATION *t;

  t = list_start;
  while (TRUE) {
    if (!CompAX25Addr(&t->call,call))
      return(t);
    if (t->next)
      t = t->next;
    else
      return(NULL);
  }
}

/*#+func-----------------------------------------------------------------------
    FUNCTION: add_station()    (Was: SLSortStore() )
     PURPOSE: Store a pointer in a Single Linked Sorted list.
      SYNTAX: DATA *SLSortStore(DATA *info, DATA *start, DATA **last)
 DESCRIPTION: See 'Advance Modula-2' pp 90
     RETURNS: Pointer to starting point
     HISTORY: 05-Apr-1993 20:18:19 V0.1
    SEE ALSO: STATION
--#-func----------------------------------------------------------------------*/
STATION *add_station(STATION *info, STATION *start, STATION **last)
{
  STATION *old, *top;
  int done = 0;

  top = start;
  old = NULL;
  done = 0;

  if (start == NULL) {
    /*---------------------------------------
      First element in the list
    ---------------------------------------*/
    info->next = NULL;
    *last = info;
    return(info);
  }
  else {
    while (start != NULL && !done) {
      if (memcmp(&start->call,&info->call,7) < 0) {
        old = start;
        start = start->next;
      }
      else {
        /*---------------------------------------
          Goes in the middle
        ---------------------------------------*/
        if (old != NULL) {
          old->next = info;
          info->next = start;
          return(top);      /* Keep same starting point */
        }
        else {
          info->next = start;
          return(info);     /* New first element */
        }
      }
    }
    if (!done) {
      (*last)->next = info;    /* Goes on the end */
      info->next = NULL;
      *last = info;
      return(top);
    }
  }
}

/*-#+func----------------------------------------------------------
    FUNCTION: LogInit()
     PURPOSE: Initialize the logging system
      SYNTAX: void LogInit(void);
 DESCRIPTION:
     RETURNS: nothing
     HISTORY: 940222 V0.1
--#-func----------------------------------------------------------*/
void LogInit()
{
  if (logfile == NULL) {
    if((logfile = fopen(logname,"a")) == NULL) {
      perror("Can't open logfile");
      return;
    }
    atexit(LogExit);
    fprintf(logfile,"\n%s\n",gr_line);
    fprintf(logfile,"PACKMON - HvA's packet monitor V%s\n",VERSION);
  }
}

/*-#+func----------------------------------------------------------
    FUNCTION: LogExit()
     PURPOSE: Exit the logging system
      SYNTAX: void LogExit(void);
 DESCRIPTION:
     RETURNS: Nothing
     HISTORY: 940328 V0.1
--#-func----------------------------------------------------------*/
void LogExit()
{
  if (fclose(logfile) == EOF)
    perror("Can't close logfile");
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
--#-func----------------------------------------------------------*/
void LogPacket(struct ax25_level1 *p1)
{
    struct ax25_packet  *p2;

#ifdef HEX
    int i;
    for(i=0; i<p1.len; i++)
        if (disklog) fprintf(outfile,"%X ",p1.data[i]);
    if (disklog) fprintf(outfile,"\n");
#endif

    p2 = ax25rec(p1);        /* Convert level1 packet to level 2 */
    if (logfile)
      if (disklog) fprintf(logfile,"%s",DumpAX25(p2,DUMP_HEADER|DUMP_INFO));
    free(p2);
}

/*-#+func----------------------------------------------------------
    FUNCTION: log_bindata()
     PURPOSE:
      SYNTAX: FILE *log_bindata(FILE *file, struct ax25_packet *p);
 DESCRIPTION: example: #BIN#3944#|37936#$1C7DA29C#D:\SP7\REM\CONFIG.ARJ
              if parameter file == NULL, a new file will be opened

 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    Sequence of events:
    1. Receiver enters ESC RB filename.
    2. Sender enters ESC SB filename.
    3. Sending program transmits "#BIN#n" where 'n' is the number of
       bytes in the file.
    4. Receiving program transmits "#OK#".
    5. Sending program transmits file.
    6. When finished (byte count decremented to zero), receiving
       program closes the file and transmits "#OK#nnnn" where 'nnnn'
       is a decimal 16-bit CRC.  This value is also displayed on the
       console screen at the end of send or receive.

    If the transfer is aborted, the incomplete file is deleted.
    The file is also deleted if the CRC check fails.

    The exact IDs exchanged by the two partners are:

    #BIN#nnnnn#|ccccc$xxxxx#fn#
        nnnnn = length in bytes
        ccccc = CRC of the whole file
        xxxxx = packed file date and time
        fn    = file name

    #OK#fn#
        fn    = file name
 ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

     RETURNS: pointer to file
     HISTORY: 940419 V0.1
--#-func----------------------------------------------------------*/

FILE *log_bindata(FILE *file, struct ax25_packet *p)
{
  FILE *f;
  char s[270];

  /*-------------------------------
    Open file if new file requested
  --------------------------------*/
  if (file == NULL) {
    if((f = fopen("datafile.dat","wb")) == NULL) {
      sprintf(s,"Can't open datafile %s","datafile.dat");
      perror(s);
      return(NULL);
    }
  }
  else
    f = file;

  /*-------------------------------
    Dump info to file and return
    pointer to file
  --------------------------------*/
  if (disklog) fprintf(f,"%s",DumpAX25(p,DUMP_INFO));
  return(f);

}

void log(char *s){
  if (logfile)
  /*
    fprintf(logfile,"%s\n",s);
  */
    fprintf(logfile,"%s\n",s);

}

