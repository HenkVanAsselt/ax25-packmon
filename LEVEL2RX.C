/*
	level2rx.c -- Level 2 AX.25 packet processing

  Poor Man's Packet (PMP)
  Copyright (c) 1991 by Andrew C. Payne    All Rights Reserved.

  Permission to use, copy, modify, and distribute this software and its
  documentation without fee for NON-COMMERCIAL AMATEUR RADIO USE ONLY is hereby
  granted, provided that the above copyright notice appear in all copies.
  The author makes no representations about the suitability of this software
  for any purpose.  It is provided "as is" without express or implied warranty.

	September, 1989
	 Andrew C. Payne
*/

/* ----- Includes ----- */

#include <stdio.h>
#include <malloc.h>
#include <dos.h>
#include "packmon.h"

/* addrtous(p2)
	Given a pointer to a level 2 packet, returns TRUE if the packet was
	addressed to us.
*/
/*
static int addrtous(struct ax25_packet *p2)
{
	int	i;

	if(CompAX25Addr(&p2->dest,&MyCall))
		return FALSE;

	if(p2->ndigis) {
		for(i=0; i<p2->ndigis; i++) {
			if(!p2->repeated[i])
				return FALSE;
		}
	}
	return TRUE;
}
*/

/* AX25Level2(l1)
	This routine is the level 2 upcall.   This routine is called once
	for each recieved level1 packet.

	This routine handles logging packets in the debugging log,
	dumping packets when not connected, and passing packets to the LAPB
	routines.
*/
void AX25Level2(struct ax25_level1 *p)
{
	struct	ax25_packet	*p2;

	if((p2 = AX25L1toL2(p)) == NULL)	/* bad packet */
		return;

#ifdef TRACE
	if(DebugMode)
		LogPacket(p,1); 		/* log incoming */
#endif
/*
	Heard(p2);				/* nodes heard */

	if(!Connected())
		ShowLevel2(p2);

	if(addrtous(p2))
		AX25_Incoming(p2);
*/
	free(p2);
}
