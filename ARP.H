/* Mods by PA0GRI */
#ifndef	_ARP_H
#define	_ARP_H

/* Lifetime of a valid ARP entry */
#define	ARPLIFE		900	/* 15 minutes */
/* Lifetime of a pending ARP entry */
#define	PENDTIME	15	/* 15 seconds */
/* Maximum number of datagrams in queue while pending an resolution */
#define ARP_QUEUE   5   /* 5 packets max on queue */

/* ARP definitions (see RFC 826) */

#define	ARPLEN	16		/* Size of ARP hdr, minus hardware addresses */

/* Address size definitions */
#define	IPALEN	4		/* Length in bytes of an IP address */
#define	MAXHWALEN	255	/* Maximum length of a hardware address */

/* #+data: ARP opcodes */
#define	ARP_REQUEST	1
#define	ARP_REPLY	2
#define	REVARP_REQUEST	3
#define	REVARP_REPLY	4
/* #-data */

/* #+data: Hardware types */
#define	ARP_NETROM	0	/* Fake for NET/ROM (never actually sent) */
#define	ARP_ETHER	1	/* Assigned to 10 megabit Ethernet */
#define	ARP_EETHER	2	/* Assigned to experimental Ethernet */
#define	ARP_AX25	3	/* Assigned to AX.25 Level 2 */
#define	ARP_PRONET	4	/* Assigned to PROnet token ring */
#define	ARP_CHAOS	5	/* Assigned to Chaosnet */
#define	ARP_IEEE802	6	/* Who uses this? */
#define	ARP_ARCNET	7
#define	ARP_APPLETALK	8
/* #-data */

#define	NHWTYPES 9

/* Table of hardware types known to ARP */
struct arp_type {
	int16 hwalen;		/* Hardware length */
	int16 iptype;		/* Hardware type field for IP */
	int16 arptype;		/* Hardware type field for ARP */
	int16 rarptype;		/* Hardware type field for RARP */
	int16 pendtime;		/* # secs to wait pending response */
	char *bdcst;		/* Hardware broadcast address */
	#ifdef DONT_SKIP_THIS
	char *(*format) __ARGS((char *,char *));	/* Function that formats addresses */
	int (*scan) __ARGS((char *,char *));        /* Reverse of format */
	#endif
}
ARP_TYPE;

/* extern struct arp_type far Arp_type[]; */
#define	NULLATYPE	(struct arp_type *)0

/* Format of an ARP request or reply packet. From p. 3 */
struct arp {
	int16 hardware;			/* Hardware type */
	int16 protocol;			/* Protocol type */
	char hwalen;			/* Hardware address length, bytes */
	char pralen;			/* Length of protocol address */
	int16 opcode;			/* ARP opcode (request/reply) */
	char shwaddr[MAXHWALEN];	/* Sender hardware address field */
	int32 sprotaddr;		/* Sender Protocol address field */
	char thwaddr[MAXHWALEN];	/* Target hardware address field */
	int32 tprotaddr;		/* Target protocol address field */
}
ARP;

/* Format of ARP table */
struct arp_tab {
	struct arp_tab *next;	/* Doubly-linked list pointers */
	struct arp_tab *prev;
	/* struct timer timer;	 *//* Time until aging this entry */
	struct mbuf *pending;	/* Queue of datagrams awaiting resolution */
	int32 ip_addr;		/* IP Address, host order */
	int16 hardware;		/* Hardware type */
	char state;		/* (In)complete */
#define	ARP_PENDING	0
#define	ARP_VALID	1
	char pub;		/* Respond to requests for this entry? */
	char *hw_addr;		/* Hardware address */
    struct iface *iface;    /* Interface to use route on -- sm6rpz */
}
ARP_TAB;

#define	NULLARP	(struct arp_tab *)0
extern struct arp_tab *Arp_tab[];

struct arp_stat {
	unsigned recv;		/* Total number of ARP packets received */
	unsigned badtype;	/* Incoming requests for unsupported hardware */
	unsigned badlen;	/* Incoming length field(s) didn't match types */
	unsigned badaddr;	/* Bogus incoming addresses */
	unsigned inreq;		/* Incoming requests for us */
	unsigned replies;	/* Replies sent */
	unsigned outreq;	/* Outoging requests sent */
}
ARP_STAT;

extern struct arp_stat Arp_stat, Rarp_stat;

/* In arphdr.c: */
int ntoharp(struct arp *arp, MBUF *buf);

/* In arpdump.c: */
char *inet_ntoa(int32 a);
char *arp_dump(MBUF *bpp, BYTE flags);


/* In arp.c: */
#ifdef DONT_SKIP_THIS
struct arp_tab *arp_add __ARGS((int32 ipaddr,int16 hardware,char *hw_addr,
    int pub,struct iface *iface));
void arp_drop __ARGS((void *p));
int arp_init __ARGS((unsigned int hwtype,int hwalen,int iptype,int arptype,
	int pendtime,char *bdcst,char *(*format) __ARGS((char *,char *)),
	int  (*scan) __ARGS((char *,char *)) ));
void arp_input __ARGS((struct iface *iface,struct mbuf *bp));
struct arp_tab *arp_lookup __ARGS((int16 hardware,int32 ipaddr,struct iface *iface));
char *res_arp __ARGS((struct iface *iface,int16 hardware,int32 target,struct mbuf *bp));

/* In rarp.c: */
void rarp_input __ARGS((struct iface *iface,struct mbuf *bp));

#endif

#endif /* _ARP_H */

