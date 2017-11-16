/********************************************************
   FILE: ARPDUMP.C
   DESC: Arp packet dump routines
   HIST: 940324 V0.1

*********************************************************/

/*-----/ Include files /-----*/

#include <stdio.h>
#include <string.h>

#include "buf.h"
#include "misc.h"
#include "packmon.h"
#include "ax25.h"
#include "arp.h"

/*-----/ Local defines /-----*/

#define NULLCHAR '\0'
#define	EADDR_LEN	6

/* Ethernet type fields */
#define	IP_TYPE		0x800	/* Type field for IP */
#define ARP_TYPE    0x806   /* Type field for ARP */
#define REVARP_TYPE 0x8035  /* Type field for reverse ARP */
#define AADDR_LEN   1

/* ARCnet type fields */
#define	ARC_IP		0xf0	/* Type field for IP */
#define	ARC_ARP		0xf1	/* Type field for ARP */

/*-----/ Local variables /-----*/

static char *Arptypes[] = {
	"NET/ROM",
	"10 Mb Ethernet",
	"3 Mb Ethernet",
	"AX.25",
	"Pronet",
	"Chaos",
	"",
	"Arcnet",
	"Appletalk"
};

static struct arp_type Arp_type[NHWTYPES] = {
    AXALEN, 0, 0, 0, 0, NULLCHAR,                       /* ARP_NETROM    */
    EADDR_LEN,IP_TYPE,ARP_TYPE,REVARP_TYPE,1,NULL,      /* ARP_ETHER     */
    0, 0, 0, 0, 0, NULLCHAR,                            /* ARP_EETHER    */
    AXALEN, PID_IP, PID_ARP, PID_RARP, 10, NULL,        /* ARP_AX25      */
    0, 0, 0, 0, 0, NULLCHAR,                            /* ARP_PRONET    */
    0, 0, 0, 0, 0, NULLCHAR,                            /* ARP_CHAOS     */
    0, 0, 0, 0, 0, NULLCHAR,                            /* ARP_IEEE802   */
    AADDR_LEN, ARC_IP, ARC_ARP, 0, 1, NULL,             /* ARP_ARCNET    */
    0, 0, 0, 0, 0, NULLCHAR,                            /* ARP_APPLETALK */
};


/*-#+func----------------------------------------------------------
    FUNCTION: inet_ntoa()
     PURPOSE: Convert an internet address (in host byte order)
              to a dotted decimal ascii string, e.g., 255.255.255.255\0
      SYNTAX:
 DESCRIPTION:
     RETURNS:
     HISTORY:
--#-func----------------------------------------------------------*/
char *inet_ntoa(int32 a)
{
  static char buf[25];

  sprintf(buf,"%u.%u.%u.%u",
              hibyte(hiword(a)),
              lobyte(hiword(a)),
              hibyte(loword(a)),
              lobyte(loword(a)) );
  return buf;
}

/*-#+func----------------------------------------------------------
    FUNCTION: ntoharp()
     PURPOSE: Convert an incoming ARP packet into a host-format structure
      SYNTAX: int ntoharp(struct arp *arp, MBUF *buf);
 DESCRIPTION:
     RETURNS: Always 0
     HISTORY: 940306 V0.1
--#-func----------------------------------------------------------*/
int ntoharp(struct arp *arp, MBUF *buf)
{
	arp->hardware = pull16(buf);
	arp->protocol = pull16(buf);
	arp->hwalen   = pull8(buf);
	arp->pralen   = pull8(buf);
	arp->opcode   = pull16(buf);
	buf2data(arp->shwaddr,buf,(int16)uchar(arp->hwalen));
	arp->sprotaddr = pull32(buf);
	buf2data(arp->thwaddr,buf,(int16)uchar(arp->hwalen));
	arp->tprotaddr = pull32(buf);

	return 0;
}

/*-#+func----------------------------------------------------------
    FUNCTION: arp_dump()
     PURPOSE: Dump arp packet
      SYNTAX: char *arp_dump(FILE *fp, MBUF *bpp, BYTE flags);
 DESCRIPTION: Dumps ARP packet 'bpp' to FILE 'fp'
     RETURNS: Nothing
     HISTORY: 940324 V0.1
--#-func----------------------------------------------------------*/
char *arp_dump(MBUF *bpp, BYTE flags)
{
    static char s[512];
    struct arp arp;
    struct arp_type *at;
    int    is_ip = 0;
    char   t[80];

    /*-----/ Dump header /-----*/
    strcpy(s,"ARP: ");

    /*-----/ Decode header to struct arp /-----*/
    if(ntoharp(&arp,bpp) == -1){
        strcat(s," bad packet\n");
        return(s);
    }

    /*-----/ Determine hardware /-----*/
    if(arp.hardware < NHWTYPES)
      at = &Arp_type[arp.hardware];
    else
      at = NULLATYPE;

    /* Print hardware type in Ascii if known, numerically if not */
    sprintf(t," %s",smsg(Arptypes,NHWTYPES,arp.hardware));
    strcat(s,t);

    /* Print hardware length only if unknown type, or if it doesn't match
     * the length in the known types table
     */
    if(at == NULLATYPE || (arp.hwalen != at->hwalen)) {
      sprintf(t," hwlen %u",arp.hwalen);
      strcat(s,t);
    }

    /* Check for most common case -- upper level protocol is IP */
    if(at != NULLATYPE && arp.protocol == at->iptype){
        strcat(s," IP");
        is_ip = 1;
    } else {
        sprintf(t," prot 0x%x prlen %u",arp.protocol,arp.pralen);
        strcat(s,t);
    }

    /*-----/ Dump ARP opcode /-----*/
    switch(arp.opcode){
    case ARP_REQUEST:
        strcat(s," REQUEST");
        break;
    case ARP_REPLY:
        strcat(s," REPLY");
        break;
    case REVARP_REQUEST:
        strcat(s," REVERSE REQUEST");
        break;
    case REVARP_REPLY:
        strcat(s," REVERSE REPLY");
        break;
    default:
        sprintf(t," op %u",arp.opcode);
        strcat(s,t);
        break;
    }
    strcat(s,"  ");

    /*-----/ Dump sender /-----*/
    if(is_ip) {
      sprintf(t,"%s->",inet_ntoa(arp.sprotaddr));
      strcat(s,t);
    }
    else
      strcat(s,"?   ");

    /*-----/ Dump target /-----*/
    if(is_ip) {
      sprintf(t,"%s",inet_ntoa(arp.tprotaddr));
      strcat(s,t);
    }
    else
      strcat(s,"?");

    /*-----/ Return pointer to generated string /-----*/
    return(s);
}


