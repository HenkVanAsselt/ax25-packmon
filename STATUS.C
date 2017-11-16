/***#+FILE********************************************************************

    FILE: STATUS.C
    DESC: Status Routines for HvA Packet Monitor
    HIST: 940328 V0.1

****#-FILE********************************************************************/

/* ----- Includes files ----- */

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <window.h>
#include "packmon.h"
#include "config.h"
#include "status.h"

/*-#+func----------------------------------------------------------
    FUNCTION: DumpStatus()
     PURPOSE: Dumps the current Packmon status variables to the log file.
      SYNTAX: void DumpStatus();
 DESCRIPTION:
     RETURNS:
     HISTORY: 940326 V0.2 - Now dumps status to fileptr
              940328 V0.3 - Dumps status to HvA window
--#-func----------------------------------------------------------*/
void DumpStatus()
{
    long    t;
    struct tm   *tm;

    time(&t);
    tm = localtime(&t);

    wn_locate(w_stat,0,0);
    wn_printf(w_stat,"STATUS  %02d/%02d/%02d %02d:%02d:%02d  ",
        tm->tm_mon+1, tm->tm_mday, tm->tm_year, tm->tm_hour,
        tm->tm_min, tm->tm_sec);
    wn_printf(w_stat,"good: %-4ld  ",RXCount);
    wn_printf(w_stat,"bad: %-4ld  ",RXCRCErr);
    wn_printf(w_stat,"  hex debug %s",hex_debug?"ON ":"OFF");
    wn_printf(w_stat,"  disk logging %s", disklog?"ON ":"OFF");
    wn_printf(w_stat,"  %s",VERSION);
}



