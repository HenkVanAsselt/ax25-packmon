/*#+File*********************************************
*
* FILE: LOG.H
* DESC: Header file for LOG.C
* HIST: 940328 V0.1
*
**#-File*********************************************/

/*-----/ DEFINES /-----*/

#define MAXID    40     /* max length of broadcast ID */

/*-----/ TYPEDEFS /-----*/

/* #+data: packet_log */
typedef struct packet_log {
    struct ax25_addr    call;       /* callsign of node           */
    long                time;       /* time sent                  */
    BYTE                s_nr;       /* Sended number              */
    FILE                *fptr;      /* File pointer               */
    long                count;      /* # packets rec'd  */
    long                bytes;      /* # bytes sent     */
    long                flags;      /* node flags       */
    BYTE                id[MAXID];  /* node ID          */
    char                path[80];   /* Path of station  */
    int                 dir;        /* direction                  */
    struct  packet_log  *next;      /* next packet in linked list */
    struct  ax25_level1 p;          /* the level 1 packet         */
}
STATION;
/* #-data */

#ifdef LOG_C    /* Called from main file LOG.C */

STATION *list_start = NULL;
STATION *list_end   = NULL;

#endif /* LOG_C */

/*-----/ PROTOTYPES /-----*/

void LogInit(void);
void LogExit(void);
void LogPacket(struct ax25_level1 *p);
void log(char *s);


