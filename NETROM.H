/*ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
³ FILE: NETROM.H                                              ³
³ DESC: Headerfile for NETROM routines                        ³
³ DATE: 940311 V0.1                                           ³
ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ*/

#include "ax25.h"

/*ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
³   STRUCTURES      ³
ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ*/
typedef struct nr_broadcast {   /* Net/Rom broadcast */
    struct ax25_addr dest;      /* destination */
    char   alias[6];            /* node identifier */
    struct ax25_addr neighbor;  /* best neighbor */
    BYTE   quality;             /* quality to this neighbor */
}
NR_BROADCAST;

/*ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
³   PROTOTYPES      ³
ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ*/

/* NETDUMP.C */

char *NetRomDump(MBUF *bpp, BYTE flags);

/*ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
³   NET/ROM SUPPORT DEFINITIONS   ³
ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ*/

#define NR3HLEN		15	    /* length of a net/rom level 3 hdr, */
#define NR3DLEN		241	    /* max data size in net/rom l3 packet */
#define NR3NODESIG	0xff	/* signature for nodes broadcast */
#define NR3POLLSIG  0xfe    /* signature for route poll - WG7J */
#define NR3NODEHL   7       /* nodes bc header length */

#define NRNUMCHAINS	17	    /* number of chains in the */
				            /* neighbor and route hash tables */
#define NRRTDESTLEN	21	    /* length of destination entry in */
				            /* nodes broadcast */
#define NRDESTPERPACK	11	/* maximum number of destinations per */
				            /* nodes packet */

/* minimum length of NET/ROM transport header */
#define	NR4MINHDR	5

/*ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
³   NET/ROM PROTOCOL NUMBERS      ³
ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ*/
#define NRPROTO_IP	0x0c

/* protocol limitation: */

#define	NR4MAXINFO	236		/* maximum data in an info packet */

/* sequence number wraparound mask */

#define NR4SEQMASK	0xff	/* eight-bit sequence numbers */

/* flags in high nybble of opcode byte */

#define	NR4CHOKE	0x80
#define	NR4NAK		0x40
#define	NR4MORE		0x20	/* The "more follows" flag for */
							/* pointless packet reassembly */

#define	NR4OPCODE	0x0f    /* mask for NETROM opcode nibble */

/* opcodes */

#define NR4OPPID	0		/* protocol ID extension to network layer */
#define	NR4OPCONRQ	1		/* connect request */
#define	NR4OPCONAK	2		/* connect acknowledge */
#define	NR4OPDISRQ	3		/* disconnect request */
#define	NR4OPDISAK	4		/* disconnect acknowledge */
#define	NR4OPINFO	5		/* information packet */
#define	NR4OPACK	6		/* information ACK */
#define NR4NUMOPS	7		/* number of transport opcodes */
