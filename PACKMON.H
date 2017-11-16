/*#+File*********************************************
*
* FILE: PACMON.H
*
* DESC: HvA Packet Monitor include file
*
* HIST:  Tue 940118 V0.1
*
**#-File*********************************************/

#include <assert.h>

#ifndef FALSE
#define FALSE 0
#define TRUE !FALSE
#endif

#define VERSION "V1.1"

#define EXTERN extern
#define WORD unsigned int
#define BYTE unsigned char

#define uchar(x) ((unsigned char)(x))

/* string equality shorthand */
#define STREQ(x,y) (strcmp(x,y) == 0)

/* Extract a short from a long */
#define	hiword(x)	((int16)((x) >> 16))
#define	loword(x)	((int16)(x))

/* Extract a byte from a short */
#define	hibyte(x)	((unsigned char)((x) >> 8))
#define	lobyte(x)	((unsigned char)(x))

/* Extract nibbles from a byte */
#define	hinibble(x)	(((x) >> 4) & 0xf)
#define	lonibble(x)	((x) & 0xf)

#define NETROM
/*
#include "pmp\dqueue.h"
*/

/*------------------------
  Prototypes
-------------------------*/

void ComputeSecondaryPar(void);
void Initialize_Timer(void);
void Restore_Timer(void);

void Initialize_COM(void);
void Restore_COM(void);

void reset_variables(void);

EXTERN BYTE com_irq;       /* COM port IRQ                                 */
EXTERN WORD com_base;      /* COM port base                                */

/* Statistics */

EXTERN WORD RxFrameLen;

EXTERN BYTE irq_mask;      /* IRQ mask for 8259                       */

EXTERN WORD RxBuffer[];
EXTERN BYTE RxFrameData[];
EXTERN WORD Ptr2;
EXTERN WORD write_ptr;
EXTERN BYTE Upcall;
EXTERN BYTE databuf[];

/* ----- Connection States ----- */
#define DISCONNECTED    0   /* disconnected mode */
#define SETUP           1   /* link setup */
#define DISCONNECTPEND  2   /* pending disconnect */
#define CONNECTED       3   /* link connected */
#define RECOVERY        4   /* link recovery */

/* ----- Line Termination Conventions ----- */
#define EOL_CR          1   /* lines terminate with CR */
#define EOL_LF          2   /* lines terminate with LF */
#define EOL_CRLF        3   /* lines terminate with CR/LF */

/* ----- Link Types ------ */
#define BINARY          1   /* binary stream */
#define TEXT            2   /* text stream */

/* packmon.c  */
void debug(char *format,...);

/* ax25dump.c */
extern void ShowLevel2(struct ax25_packet *p2);
extern char *DumpLevel2(struct ax25_packet *p2);

/* ax25subr.c */
extern int SetAX25Addr(struct ax25_addr *a, char *s);
extern char *GetAX25Addr(struct ax25_addr *p);
int CompAX25Addr(struct ax25_addr *a1, struct ax25_addr *a2);
extern int FrameType(BYTE c);
extern int CmdResp(struct ax25_level1 *p);
extern int SetAX25Path(char *s, struct ax25_packet *a);
extern char *GetAX25Path(struct ax25_packet *p);
extern struct ax25_packet *AX25L1toL2(struct ax25_level1 *p);
extern int SendAX25(struct ax25_packet *p);
extern void ReversePath(struct ax25_packet *p1, struct ax25_packet *p2);
extern void SendBeacon(void);
extern void StartBeacon(void);
extern void SendWelcome(void);

/* misc.c */
extern char *sob(char *s);
extern char *extract(char *s, char *p);
extern int convert(char *dest, char *src);
extern int dispatch(char *s, struct param_cmd *pcmds);
extern void trim(char *s);
extern void eol_out(int conv, char *src, int len);
extern void eol_in(int conv, char *src, int len);

/* help.c */
void DoHelp(void);

/* key.c */
int handle_key(void);

/* ----- Counters ----- */
EXTERN long RXCount;                    /* number of good frames received */
EXTERN long RXCRCErr;                   /* number of CRC error received */

