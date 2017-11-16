/* Mods by PA0GRI (WFM IP access code ) */
#ifndef	_IP_H
#define	_IP_H

/* IP header, INTERNAL representation */

#define IPLEN		20	/* Length of standard IP header */
#define IP_MAXOPT	40	/* Largest option field, bytes */

typedef struct ip {
	int32 source;		/* Source address */
	int32 dest;		    /* Destination address */
	int16 length;		/* Total length */
	int16 id;		    /* Identification */
	int16 offset;		/* Fragment offset in bytes */
	int16 checksum;		/* Header checksum */

	struct {
		char congest;	/* Congestion experienced bit (exp) */
		char df;	    /* Don't fragment flag */
		char mf;	    /* More Fragments flag */
	} flags;

	char version;		/* IP version number */
	char tos;		    /* Type of service */
	char ttl;		    /* Time to live */
	char protocol;		/* Protocol */
	char optlen;		/* Length of options field, bytes */
	char options[IP_MAXOPT];/* Options field */
}
IP;

#define	NULLIP (IP *)0

/* Fields in option type byte */
#define	OPT_COPIED	0x80	/* Copied-on-fragmentation flag */
#define	OPT_CLASS	0x60	/* Option class */
#define	OPT_NUMBER	0x1f	/* Option number */

/* IP option numbers */
#define	IP_EOL		0	/* End of options list */
#define	IP_NOOP		1	/* No Operation */
#define	IP_SECURITY	2	/* Security parameters */
#define	IP_LSROUTE	3	/* Loose Source Routing */
#define	IP_TIMESTAMP	4	/* Internet Timestamp */
#define	IP_RROUTE	7	/* Record Route */
#define	IP_STREAMID	8	/* Stream ID */
#define	IP_SSROUTE	9	/* Strict Source Routing */

/* Timestamp option flags */
#define	TS_ONLY		0	/* Time stamps only */
#define	TS_ADDRESS	1	/* Addresses + Time stamps */
#define	TS_PRESPEC	3	/* Prespecified addresses only */

/* IP routing table entry */
struct route {
	struct route *prev;	/* Linked list pointers */
	struct route *next;
	int32 target;		/* Target IP address */
	unsigned int bits;	/* Number of significant bits in target */
	int32 gateway;		/* IP address of local gateway for this target */
	int32 metric;		/* Hop count or whatever */
	struct iface *iface;	/* Device interface structure */
	int flags;
#define	RTPRIVATE	0x1	/* Should the world be told of this route ? */
#define	RTTRIG	0x2		/* Trigger is pending for this route */
	/*struct timer timer;*/	/* Time until aging of this entry */
	int32 uses;		/* Usage count */
};

#define	NULLROUTE	(struct route *)0
/* extern struct route *Routes[32][HASHMOD]; */ /* Routing table */
extern struct route R_default;			/* Default route entry */

/* IP access header entry  (WFM) */
struct rtaccess{
	struct rtaccess *nxtiface;	/* Linked list pointer */
	struct rtaccess *nxtbits;
	int16 status;		/* 0=permit, 1=deny */
	int16 protocol;		/* 0=any, otherwise IP protocol # */
	int32 source;		/* Source IP address */
	unsigned int sbits;	/* Number of significant bits in source */
	int32 target;		/* Target IP address */
	unsigned int bits;	/* Number of significant bits in target */
	struct iface *iface;	/* Device interface structure */
	int16 lowport;		/* tcp & udp port range. low=0 implies all */
	int16 highport;
};
#define NULLACCESS	(struct rtaccess *)0
extern struct rtaccess *IPaccess;
/* end WFM access control */

/* Cache for the last-used routing entry, speeds up the common case where
 * we handle a burst of packets to the same destination
 */
struct rt_cache {
	int32 target;
	struct route *route;
};
extern struct rt_cache Rt_cache;

/* Reassembly descriptor */
struct reasm {
	struct reasm *next;	/* Linked list pointer */
	/* struct timer timer; */	/* Reassembly timeout timer */
	struct frag *fraglist;	/* Head of data fragment chain */
	int16 length;		/* Entire datagram length, if known */
	int32 source;		/* src/dest/id/protocol uniquely describe a datagram */
	int32 dest;
	int16 id;
	char protocol;
};
#define	NULLREASM	(struct reasm *)0

/* Fragment descriptor in a reassembly list */
struct frag {
	struct frag *prev;	/* Previous fragment on list */
	struct frag *next;	/* Next fragment */
	struct mbuf *buf;	/* Actual fragment data */
	int16 offset;		/* Starting offset of fragment */
	int16 last;		/* Ending offset of fragment */
};

int get_IP_hdr(struct ip *ipbuf, char *buf, int len);
char *ip_dump(BYTE *d, int len);

#endif /* _IP_H */
