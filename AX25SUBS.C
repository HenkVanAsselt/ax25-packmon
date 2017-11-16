/*
	AX25DUMP.C -- Dumps AX.25 packets in human readable format

  Poor Man's Packet (PMP)
  Copyright (c) 1991 by Andrew C. Payne    All Rights Reserved.

  Permission to use, copy, modify, and distribute this software and its
  documentation without fee for NON-COMMERCIAL AMATEUR RADIO USE ONLY is hereby
  granted, provided that the above copyright notice appear in all copies.
  The author makes no representations about the suitability of this software
  for any purpose.  It is provided "as is" without express or implied warranty.

	August, 1989
	Andrew C. Payne

	09/13/89 - changed to work with L2 packet structures /acp/
*/

/* ----- Includes ----- */
#include <stdio.h>
#include <conio.h>
#include <memory.h>
#include <malloc.h>
#include <string.h>
#include "packmon.h"
#ifdef NETROM
#include "pmp\netrom.h"
#endif

/*-#+func----------------------------------------------------------------
    FUNCTION: FrameType()
     PURPOSE: Given the control byte, returns the type of the frame.
      SYNTAX: int FrameType(BYTE c);
 DESCRIPTION:
     RETURNS: Type of the given frame
     HISTORY:
--#-func----------------------------------------------------------------*/
int FrameType(BYTE c)
{
	if(!(c & 1))
		return I;	        /* Information */

	if(c & 2)
		return c & ~PF;		/* U frames, strip PF */
	else
		return c & 0xf;		/* S frames */
}

