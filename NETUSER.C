/* Miscellaneous interger and IP address format conversion subroutines
 * Copyright 1991 Phil Karn, KA9Q
 */

#include <ctype.h>
#include <stdio.h>
#include "netuser.h"

/*-#+func----------------------------------------------------------
    FUNCTION: inet_ntoa()
     PURPOSE: Convert an internet address (in host byte order) to a
              dotted decimal ascii string, e.g., 255.255.255.255\0
      SYNTAX: char *inet_ntoa(int32 a);
 DESCRIPTION:
     RETURNS: Pointer to buffer, holding the IPaddres string
     HISTORY: 940228 V0.1
------------------------------------------------------------------*/
char *inet_ntoa(int32 a)
{
    static char buf[25];
    char *name;

    sprintf(buf,"%u.%u.%u.%u",
        hibyte(hiword(a)),
        lobyte(hiword(a)),
        hibyte(loword(a)),
        lobyte(loword(a)) );
    return buf;
}

