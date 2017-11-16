/**********************************************
*
*  FILE: IPHDR.C
*
*  PURPOSE:  IP header conversion routines
*
*  Copyright 1991 Phil Karn, KA9Q
************************************************/

#include <stdio.h>
#include <string.h>

#include "packmon.h"
#include "buf.h"
#include "ip.h"
#include "internet.h"
#include "arp.h"

/*-#+func----------------------------------------------------------
    FUNCTION: get_IP_hdr()
     PURPOSE: Extract an IP header from buffer
      SYNTAX: int get_IP_hdr(IP *ipbuf, char *buf, int len);
 DESCRIPTION:
     RETURNS: TRUE if succesfull, FALSE if not
     HISTORY: 940627 V0.1
--#-func----------------------------------------------------------*/
int get_IP_hdr(IP *ipbuf, char *buf, int len)
{
	int ihl;
	int16 fl_offs;

/*
	if(pullup(bpp,buf,IPLEN) != IPLEN)
		return -1;
*/

	ipbuf->version = (buf[0] >> 4) & 0xf;
	debug("ip version = %d\n",ipbuf->version);
	ipbuf->tos = buf[1];
	ipbuf->length = get16(&buf[2]);
	ipbuf->id = get16(&buf[4]);
	fl_offs = get16(&buf[6]);
	ipbuf->offset = (fl_offs & 0x1fff) << 3;
	ipbuf->flags.mf = (char)((fl_offs & 0x2000) ? 1 : 0);
	ipbuf->flags.df = (char)((fl_offs & 0x4000) ? 1 : 0);
	ipbuf->flags.congest = (char)((fl_offs & 0x8000) ? 1 : 0);
	ipbuf->ttl = buf[8];
	ipbuf->protocol = buf[9];
	ipbuf->checksum = get16(&buf[10]);
	ipbuf->source = get32(&buf[12]);
	debug("ip source = %ld\n",ipbuf->source);
	ipbuf->dest = get32(&buf[16]);
	debug("ip dest   = %ld\n",ipbuf->dest  );

	ihl = (buf[0] & 0xf) << 2;
	if(ihl < IPLEN){
		/* Bogus packet; header is too short */
		debug("ihl = %d, header is too short. Should be at least %d\n",ihl,IPLEN);
		ipbuf->optlen = 0;
		return(FALSE);
	}

    len = 0;    /* BOGUS */

	return(TRUE);
}

/*-#+func----------------------------------------------------------
    FUNCTION: ip_dump()
     PURPOSE: Dump IP header
      SYNTAX: char *ip_dump(BYTE *d, int len);
 DESCRIPTION: Calls get_IP_hdr to decode IP header
     RETURNS: pointer to decoded text
     HISTORY: 940627 V0.1
--#-func----------------------------------------------------------*/
char *ip_dump(BYTE *d, int len)
{
    static char dtext[1024];
    static struct ip ip;
    int check = 0;
    char t[80];
    int ip_len = 0;     /* BOGUS */

    dtext[0] = '\0';          /* Initialize text buffer */

    if (!get_IP_hdr(&ip,d,len))
      return(dtext);          /* Return pointer to empty text buffer */

	sprintf(t," len %u",ip.length);
	strcat(dtext,t);
	sprintf(t," %s",inet_ntoa(ip.source));
	strcat(dtext,t);
	sprintf(t,"->%s ihl %u ttl %u",
		inet_ntoa(ip.dest),ip_len,uchar(ip.ttl));
	strcat(dtext,t);

	if(ip.tos != 0) {
		sprintf(t," tos %u",uchar(ip.tos));
        strcat(dtext,t);
    }
	if(ip.offset != 0 || ip.flags.mf) {
		sprintf(t," id %u offs %u",ip.id,ip.offset);
        strcat(dtext,t);
    }
	if(ip.flags.congest) {
		sprintf(t," CE");
        strcat(dtext,t);
    }
	if(ip.flags.df) {
		sprintf(t," DF");
        strcat(dtext,t);
    }
	if(ip.flags.mf){
		sprintf(t," MF");
        strcat(dtext,t);
		check = 0;	/* Bypass host-level checksum verify */
	}

/*
	if(csum != 0) {
		sprintf(t," CHECKSUM ERROR (%u)",csum);
		strcat(dtext,t);
*/
	if(ip.offset != 0){
#ifdef TNOS_68K
		fprintf (fp, "\n");
#else
 /*		putc('\n',fp);  */
#endif
		return(dtext);
	}
	switch(uchar(ip.protocol)){
	case TCP_PTCL:
		sprintf(t," prot TCP\n");
        strcat(dtext,t);
		/* tcp_dump(fp,bpp,ip.source,ip.dest,check);   */
		break;
	case UDP_PTCL:
		sprintf(t," prot UDP\n");
        strcat(dtext,t);
		/* udp_dump(fp,bpp,ip.source,ip.dest,check);   */
		break;
	case ICMP_PTCL:
		sprintf(t," prot ICMP\n");
        strcat(dtext,t);
		/* icmp_dump(fp,bpp,ip.source,ip.dest,check);  */
		break;
	case IP_PTCL:
		sprintf(t," prot IP\n");
        strcat(dtext,t);
		/* ip_dump(fp,bpp,check);   */
		break;
#ifdef AX25
    case AX25_PTCL:
		sprintf(t," prot AX25\n");
        strcat(dtext,t);
		/* ax25_dump(fp,bpp,check); */
		break;
#endif
#ifdef  RSPF
	case RSPF_PTCL:
		sprintf(t," prot RSPF\n");
        strcat(dtext,t);
		/* rspf_dump(fp,bpp,ip.source,ip.dest,check); */
		break;
#endif
	default:
		sprintf(t," prot %u\n",uchar(ip.protocol));
        strcat(dtext,t);
		break;
	}
	return(dtext);
}


