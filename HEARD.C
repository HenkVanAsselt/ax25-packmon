/***********************************************************

    FILE: HEARD.C
    DESC: Stations/nodes heard logging routines
    HIST: 940328 V0.2
          940701 V0.3 - An Entry in the heardlist is empty if the
                        number of packets received up till now is 0.

***********************************************************/

/* ----- Includes ----- */
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <time.h>
#include <memory.h>
#include <string.h>

#include <mshell.h>

#include "config.h"
#include "packmon.h"
#include "ax25.h"
#include "heard.h"
#include "misc.h"
#include "crc.h"

/*-----/ LOCAL VARIABLES /-----*/

static MHEARD *heard;           /* MHEARD list           */
static int nheard = 0;          /* number in mheard list */

/*-----/ LOCAL PROTOTYPES /-----*/
void init_entry(int i);
static int find_heard(AX25_ADDR source, AX25_ADDR dest);
static int find_oldest(void);
static int find_empty(void);
static int find_conn_status(int status);
static void del_connection(time_t interval, int status);

/*-#+func----------------------------------------------------------
    FUNCTION: find_empty()
     PURPOSE: Find first empty entry in heardlist
      SYNTAX: int find_empty(void);
 DESCRIPTION: Looks for the first connection entry with 0
              received packets.
     RETURNS: 0...MAX_HEARD-1   Index of entry found.
              HEARD_LIST_FULL   if headlist is full
     HISTORY: 940701 V0.1 - Initial version
--#-func----------------------------------------------------------*/
static int find_empty(void)
{
  int i;

  for (i=0 ; i<MAXHEARD ; i++) {
    if (heard[i].count == 0L) {   /* 0 packets received, then entry is empty */
      return(i);
    }
  }
  return(HEARD_LIST_FULL);           /* Heardlist is full */
}

/*-#+func----------------------------------------------------------
    FUNCTION: find_conn_status()
     PURPOSE: Find connection with given status (frametype)
      SYNTAX: int find_conn_status(int status);
 DESCRIPTION: -
     RETURNS: Index of first station with given status
              HEARD_NOT_FOUND if status not found in heard list.
     HISTORY: 940701 V0.1 - Initial version
--#-func----------------------------------------------------------*/
static int find_conn_status(int status)
{
  int i;

  for (i=0 ; i<MAXHEARD ; i++) {
    if (heard[i].frametype == status)
      return(i);
  }
  return(HEARD_NOT_FOUND);
}

/*-#+func----------------------------------------------------------
    FUNCTION: del_connection()
     PURPOSE: Delete connections from heardlist with given
              timeinterval
      SYNTAX: void del_connection(int interval);
 DESCRIPTION: int interval = interval in seconds since last heard
     RETURNS: nothing
     HISTORY: 940701 - Initial version
--#-func----------------------------------------------------------*/
static void del_connection(time_t interval, int status)
{
  int i;

  for (i=0 ; i<MAXHEARD ; i++) {
    if (status == 0) {
      if ((time(NULL) - heard[i].time) > interval) {
        heard[i].count = 0L;              /* Set entry as empty */
      }
    }
    else {
      if ((time(NULL) - heard[i].time) > interval && heard[i].frametype == status) {
        heard[i].count = 0L;              /* Set entry as empty */
      }
    }
  }
}

/*-#+func----------------------------------------------------------
    FUNCTION: HeardExit()
     PURPOSE: Exit from the heard list
      SYNTAX: void HeardExit(void);
 DESCRIPTION: Closes all open files and frees memory
     RETURNS: nothing
     HISTORY: 940305 V0.1 - Initial version
--#-func----------------------------------------------------------*/
void HeardExit()
{
  int i;

  /*-----/ Close open files /-----*/
  for (i=0 ; i<MAXHEARD ; i++) {
    if (heard[i].fptr)
      fclose(heard[i].fptr);
  }

  /*-----/ Free memory /-----*/
  if (heard) {
    assert(heard);
    free(heard);
  }
}

/*-#+func----------------------------------------------------------
    FUNCTION: HeardInit()
     PURPOSE: Initialize the HEARD list structure.
      SYNTAX: void HeardInit(void);
 DESCRIPTION:
     RETURNS:
     HISTORY:
--#-func----------------------------------------------------------*/
void HeardInit()
{
  int i;

  /*-----/ Allocate memory /-----*/
  heard = calloc(MAXHEARD,sizeof(MHEARD));
  assert(heard);

  if(heard == NULL) {
      fprintf(stdout,"Out Of Memory");
      exit(-1);
  }

  /*-----/ Initialize variables /-----*/
  for (i=0 ; i<MAXHEARD ; i++)
    init_entry(i);

  atexit(HeardExit);

}

/*-#+func----------------------------------------------------------
    FUNCTION: init_entry()
     PURPOSE: Initialize entry of heard list
      SYNTAX: void init_entry(int i);
 DESCRIPTION: Intializes entry i of heard list
     RETURNS: nothing
     HISTORY: 940305 V0.1 - Initial version
--#-func----------------------------------------------------------*/
void init_entry(int i)
{
  assert(i < MAXHEARD);

  heard[i].fptr = NULL;
  heard[i].bytes = 0L;
  heard[i].count = 0L;
  heard[i].flags = 0L;
  heard[i].id[0] = '\0';
  heard[i].s_nr  = 0xF;    /* Initialize sended number to number out of range 0..7 */
  heard[i].r_nr  = 0xF;    /* Initialize received number to number out of range 0..7 */
  heard[i].dup   = 0;      /* 0 duplicates heard */
  heard[i].path[0] = '\0';
  heard[i].time = time(NULL);
}

/*-#+func----------------------------------------------------------
    FUNCTION: find_heard()
     PURPOSE: find connection in the heard list
      SYNTAX: int find_heard(ax25_addr source,ax25_addr dest);
 DESCRIPTION: Does a linear search in the heard list
     RETURNS: index of connection if found in the list (0...MAXHEARD-1)
              HEARD_NOT_FOUND   if connection not found
              HEARD_LIST_FULL   if list is full
     HISTORY: 940403 V0.1 - Initial version
              940624 V0.2 - Deals with connections instead of
                            single stations
--#-func----------------------------------------------------------*/
static int find_heard(AX25_ADDR source, AX25_ADDR dest)
{
   int i;

   for (i=0 ; i<MAXHEARD ; i++) {
     if (!CompAX25Addr(&source,&heard[i].call) && !CompAX25Addr(&dest,&heard[i].dest)) {
       return(i);       /* Connection found, return index            */
     }
   }
   return(HEARD_NOT_FOUND);          /* Station not found and list full        */
}

/*-#+func----------------------------------------------------------
    FUNCTION: find_oldest()
     PURPOSE: find station with oldest timestamp in the heard list
      SYNTAX: int find_oldest(void)
 DESCRIPTION: Does linear search in heardlist
     RETURNS: Index of station with oldest time stamp
     HISTORY: 940405 V0.1 - Initial version
--#-func----------------------------------------------------------*/
static int find_oldest()
{
  int i;
  int noldest;
  time_t oldest = 0x7fffffffL;       /* Set default time  */

  for (i=0 ; i<MAXHEARD ; i++) {
    if(heard[i].time < oldest) {    /* Check time heard against oldest */
      oldest = heard[i].time;       /* Set new oldest time             */
      noldest = i;                  /* Set index to oldest station     */
    }
  }
  return(noldest);
}


/*-#+func----------------------------------------------------------
    FUNCTION: Heard()
     PURPOSE: Heard list upcall for level 2 packets.
      SYNTAX: void Heard(AX25_PACKET *p);
 DESCRIPTION: This routine is called once for all received level 2 packets.
     RETURNS: nothing
     HISTORY: 940223 V0.1
--#-func----------------------------------------------------------*/
void Heard(AX25_PACKET *p)
{
  int i = 0;
  int ret;
  int  do_init = FALSE;
  char fname[20];
  char src[10];

  /*-----/ Try to find connection in heard list /-----*/

  ret = find_heard(p->source,p->dest);
  switch(ret) {
     case HEARD_LIST_FULL:
       i = find_oldest();           /* Find oldest entry in list    */
       if (heard[i].fptr){          /* If file open                 */
         fclose(heard[i].fptr);     /* Close file of this entry     */
       }
       do_init = TRUE;              /* Set flag to initialize entry */
       break;
     case HEARD_NOT_FOUND:
       i = find_empty();
       do_init = TRUE;              /* Set flag to initialize entry */
       break;
     default:
       do_init = FALSE;
       i = ret;
       break;
   }

  /*-----/ Initialize new entry /-----*/
  if (do_init) {
    init_entry(i);          /* Initialize entry             */
    memcpy(&heard[i].call, &p->source, sizeof(AX25_ADDR));
    memcpy(&heard[i].dest, &p->dest  , sizeof(AX25_ADDR));
    strcpy(fname,GetAX25Addr(&heard[i].call));
    strcat(fname,".log");
    heard[i].fptr = fopen(fname,"a");
    if (disklog) fprintf(heard[i].fptr,"\n%s\n",gr_line);
    if (disklog) fprintf(heard[i].fptr,"Packmon logging started at %s\n",timestr(time(NULL)));
  }

  heard[i].time = time(NULL);                   /* Set time heard           */
  heard[i].count++;                             /* Adjust packet counter    */
  heard[i].frametype = FrameType(p->cont);      /* Set frametype            */

  /*-----/ Set flags according to frame type /-----*/
  switch(heard[i].frametype) {
    case I:
    case UI:
      heard[i].bytes += p->dlen;
      heard[i].flags |= HEARD_INFO;
      switch(p->pid) {
        case PID_NETROM:
            heard[i].flags |= HEARD_NETROM;   break;
        case PID_ARP:
            heard[i].flags |= HEARD_ARP;      break;
        case PID_IP:
            heard[i].flags |= HEARD_IP;       break;
        default:
            break;
      }
    default:
        break;
  }

  /*-----/ Heard station directly ? /-----*/
  if(p->ndigis == 0)
    heard[i].flags |= HEARD_DIRECT;
  else
    strcpy(heard[i].path,dump_digis(p));

  /*-----/ Heard coded transmission ? /-----*/
  if (p->dlen >= 7) {
    if (!memcmp(p->data,"//CODE",6))
      heard[i].flags |= HEARD_CODE;
  }

  /*-----/ Heard start of binary transfer ?  /-----*/
  if (p->dlen >= 5) {
    if (!memcmp(p->data,"#BIN#",5))
      heard[i].flags |= HEARD_BIN;
  }

  /*-----/ Heard mail header ? /-----*/
  if (p->dlen >= 5) {
    if (!memcmp(p->data,"Mail for",8))
      heard[i].flags |= HEARD_MAIL;
  }

  /*-----/ Heard FBB broadcast ? /-----*/
  if (!strcmp(GetAX25Addr(&p->dest),"FBB"))
    heard[i].flags |= HEARD_FBB;

  if (heard[i].frametype == RR)
    heard[i].r_nr = ((p->cont >> 5) & 7);                        /* Get received frame number   */

  if (heard[i].frametype == I || heard[i].frametype == UI) {     /* If I or UI frame            */

    /*-----/ Check if duplicate heard /-----*/
    if (heard[i].crc == 0) {
      heard[i].crc = get_CRC(p);
      heard[i].dup = 0;
    }
    else if (heard[i].crc == get_CRC(p))
      heard[i].dup++;
    else {
      heard[i].crc = get_CRC(p);
      heard[i].dup = 0;
    }

    /*-----/ Save receive and send frame numbers  /-----*/
    heard[i].s_nr = ((p->cont >> 1) & 7);                        /* Get sended frame number     */
    heard[i].r_nr = ((p->cont >> 5) & 7);                        /* Get received frame number   */

    if (!strcmp(GetAX25Addr(&p->dest),"FBB"))
      log_fbb(p);

    if (heard[i].fptr) {                                         /* and if logfile opened       */
      strcpy(src,GetAX25Addr(&p->source));
      if (disklog) fprintf(heard[i].fptr,"[%s>%s] ",src,GetAX25Addr(&p->dest));
      if (disklog) fprintf(heard[i].fptr,"%s",DumpAX25(p,DUMP_INFO));
    }
  }

}

/*-#+func----------------------------------------------------------
    FUNCTION: DoHeard()
     PURPOSE: Perform the nodes heard command
      SYNTAX: void DoHeard(FILE *f);
 DESCRIPTION:
     RETURNS:
     HISTORY: 940324 V0.2 - Now dumps to file f
--#-func----------------------------------------------------------*/
void DoHeard()
{
    int     i;
    char    s[80],t[30];
    MHEARD  *p;
    char    source[9],dest[9];

    del_connection(600L,0);         /* Delete connections of older then given timeinterval  */
    del_connection(60L,DISC);       /* Delete disconnected stations with given timeinterval */

    wn_clear(w_heard);

    /*-----/ Print header /-----*/
                /*     123456789* 123456789 12:34:56 123456 123456 SABM   S R NET/ROM  Path*/
    wn_printf(w_heard,"Source   R Destin.   Time     Count  Bytes  Status S R D Flags    Path            \n");
    wn_printf(w_heard,"컴컴컴컴컴 컴컴컴컴� 컴컴컴�  컴컴컴 컴컴컴 컴컴컴 � � � 컴컴컴컴 컴컴컴컴컴컴컴컴\n");

    /*-----/ Show the heard list /-----*/
    for(i=0; i<MAXHEARD; i++) {
      if (heard[i].count > 0)
      {
        p = &heard[i];
        strcpy(source,GetAX25Addr(&p->call));
        strcpy(dest  ,GetAX25Addr(&p->dest));
        sprintf(s,"%-9s%c %-9s %s %6ld %6ld  ",
            source,
            (p->flags & HEARD_DIRECT) ? ' ' : '*',
            dest,
            timestr(p->time),
            p->count,
            p->bytes);

        sprintf(t," %4s",get_frametype_str(heard[i].frametype));
        strcat(s,t);

        /*-----/ Display send and received frame numbers /-----*/
        if (heard[i].s_nr != 0x0F)
          sprintf(t," %1d",heard[i].s_nr);
        else
          sprintf(t,"  ");
        strcat(s,t);

        if (heard[i].r_nr != 0x0F)
          sprintf(t," %1d",heard[i].r_nr);
        else
          sprintf(t,"  ");
        strcat(s,t);

        /*-----/ Print number of duplicate frames heard /-----*/
        heard[i].dup > 0 ? sprintf(t," %1d ",heard[i].dup) : sprintf(t,"   ");
        strcat(s,t);

        /*-----/ Display flags /-----*/
        /*  if(p->flags & HEARD_INFO)   strcat(s,"Text ");    */
        if(p->flags & HEARD_IP)      strcat(s,"IP ");
        if(p->flags & HEARD_NETROM)  strcat(s,"NET/ROM ");
        if(p->flags & HEARD_CODE)    strcat(s,"CODE ");
        if(p->flags & HEARD_BIN)     strcat(s,"BIN ");
        if(p->flags & HEARD_ARP)     strcat(s,"ARP ");
        if(p->flags & HEARD_MAIL)    strcat(s,"MAIL");
        if(p->flags & HEARD_FBB)     strcat(s,"FBB");

        wn_printf(w_heard,"%-70.70s",s);

        /*-----/ Display path /-----*/
        wn_printf(w_heard,"%s\n",heard[i].path);
      }
    }
}

/*-#+func----------------------------------------------------------
    FUNCTION: log_fbb()
     PURPOSE: Log messages found in broadcast to FBB
      SYNTAX: void log_fbb(AX25_PACKET *p);
 DESCRIPTION:
     RETURNS: Nothing
     HISTORY: 940905 V0.1
    SEE ALSO: FBB
--#-func----------------------------------------------------------*/
void log_fbb(AX25_PACKET *p)
{
  char *tok;
  char *delim = " \t\n";
  char *s;
  FBB  fbb;

  debug("Heard FBB broadcast");

  /*-----/ Initialize variables /-----*/
  s = calloc(p->dlen+2,sizeof(char));
  if (!s)
    return;
  memcpy(s,p->data,p->dlen);
  s[p->dlen+1] = '\0';

  if(tok = strtok(s,delim)) {
    fbb.nr = atoi(tok);
    debug("nr = %d\n",fbb.nr);
  }
  if(tok = strtok(NULL,delim)) {
    strcpy(fbb.stat,tok);
    debug("stat = %s\n",fbb.stat);
  }
  if(tok = strtok(NULL,delim)) {
    fbb.bid = atoi(tok);
    debug("bid  = %d\n",fbb.bid);
  }
  if(tok = strtok(NULL,delim)) {
    strcpy(fbb.name,tok);
    debug("src  = %s\n",fbb.name);
  }
  if(tok = strtok(NULL,delim)) {
    strcpy(fbb.bbs,tok);
    debug("bbs  = %s\n",fbb.bbs);
  }
  if(tok = strtok(NULL,delim)) {
    strcpy(fbb.src,tok);
    debug("src  = %s\n",fbb.src);
  }
  if(tok = strtok(NULL,delim)) {
    strcpy(fbb.date,tok);
    debug("stat = %s\n",fbb.date);
  }
  if(tok = strtok(NULL,"\n")) {
    strcpy(fbb.descr,tok);
    debug("descr = %s\n",fbb.descr);
  }

  free(s);
}
