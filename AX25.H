/***************************************************************
    FILE: AX25.H
    DESC: Headerfile for AX25.C
    DATE: 940311

***************************************************************/

#ifndef _AX25_H
#define _AX25_H

/* AX.25 datagram (address) sub-layer definitions */

#define MAXDIGIS    7   /* Maximum number of digipeaters */
#define ALEN        6   /* Number of chars in callsign field */
#define AXALEN      7   /* Total AX.25 address length, including SSID */
#define AXBUF       10  /* Buffer size for maximum-length ascii call */

/*----- DUMP Flags -----*/
#define DUMP_HEADER 1
#define DUMP_INFO   2

/* AX25.C */
void   AX25Level2(struct ax25_level1 *p);
struct ax25_packet *ax25rec(struct ax25_level1 *p1);

/* AX25DUMP.C */
char *DumpAX25hdr(struct ax25_packet *p);
char *DumpAX25(struct ax25_packet *p, BYTE flags);
char *dump_digis(struct ax25_packet *p);
char *DumpText(BYTE *d, unsigned int len);

/* AX25SUBR.C */

#define MAX_MULTI_ADDR 9
#define MA_NONE        -1

#define MA_QST     0
#define MA_NODES   1
#define MA_MAIL    2
#define MA_ID      3
#define MA_OPEN    4
#define MA_CQ      5
#define MA_BEACON  6
#define MA_RMNC    7
#define MA_ALL     8

#define MAXCLEN     6   /* # of characters in callsign */

/* ----- Control field stuff ----- */
#define I       0x00        /* Information */
#define S       0x01        /* Supervisory */
#define RR      0x01        /* Receiver ready */
#define RNR     0x05        /* Receiver not ready */
#define REJ     0x09        /* Reject */
#define U       0x03        /* unnumbered frames */
#define SABM    0x2f        /* Connect request */
#define DISC    0x43        /* Disconnect */
#define DM      0x0f        /* disconnected mode */
#define UA      0x63        /* unnumbered acknowledge */
#define FRMR    0x87        /* frame reject */
#define UI      0x03        /* unnumbered information */
#define PF      0x10        /* poll/final bit */

/* ----- Command bit ----- */
#define UNKNOWN 0       /* pre-AX.25 level 2 */
#define COMMAND 1
#define RESPONSE 2

/* ----- Bit masks ----- */
#define SSIDMASK    0x1e    /* mask for SSID nibble */
#define REPEATED    0x80    /* has been repeated bit */

/* #+data: PID AX.25 Level 3 Protocol ID's (PID) bytes ----- */
#define PID_X25       0x01  /* CCITT X.25 PLP */
#define PID_SEGMENT   0x08  /* Segmentation fragment */
#define PID_TEXNET    0xc3  /* TEXNET datagram protocol */
#define PID_LQ        0xc4  /* Link quality protocol */
#define PID_APPLETALK 0xca  /* Appletalk */
#define PID_APPLEARP  0xcb  /* Appletalk ARP */
#define PID_IP        0xcc  /* ARPA Internet Protocol */
#define PID_ARP       0xcd  /* ARPA Address Resolution Protocol */
#define PID_RARP      0xce  /* ARPA Reverse Address Resolution Protocol */
#define PID_NETROM    0xcf  /* NET/ROM */
#define PID_NO_L3     0xf0  /* No level 3 protocol */
#define PID_TEXT      0xf0  /* no Level 3 */
/* #-data */

/* ----- Global Structures ----- */
typedef struct ax25_level1 {    /* Level 1 packet */
    int len;                    /* number of BYTEs */
    BYTE    data[1];            /* zero or more data bytes */
}
AX25_LEVEL1;

typedef struct ax25_addr {      /* AX.25 address */
    BYTE    call[MAXCLEN];      /* Callsign, shifted left one bit */
    BYTE    ssid;               /* sub-station ID field */
}
AX25_ADDR;

/*-------------------------------------
  Internal representation of packets
-------------------------------------*/
typedef struct ax25_packet {
    struct ax25_addr dest;                  /* destination           */
    struct ax25_addr source;                /* source                */
    struct ax25_addr digis[MAXDIGIS];       /* Digipeaters           */
    char             repeated[MAXDIGIS];    /* has been repeated     */
    int              ndigis;                /* number of digipeaters */
    BYTE             cmdresp;               /* command/response bit  */
    BYTE             cont;                  /* control BYTE          */
    BYTE             pid;                   /* protocol ID           */
    int              dlen;                  /* data length           */
    BYTE             data[1];               /* data bytes            */
}
AX25_PACKET;

int is_ax25multi(struct ax25_addr addr);

#endif   /* _AX25_H */

