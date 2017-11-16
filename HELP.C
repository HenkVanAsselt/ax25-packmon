/*#+File*********************************************
*
* FILE: help.c
*
* DESC: Packet monitor help
*
* HIST: 940609 V0.1
**#-File*********************************************/

#include <window.h>
#include "packmon.h"

/*-#+func----------------------------------------------------------
    FUNCTION: DoHelp()
     PURPOSE: Show help window
      SYNTAX: void DoHelp(void);
 DESCRIPTION: -
     RETURNS: Nothing
     HISTORY: 940609 V0.1
--#-func----------------------------------------------------------*/
void DoHelp()
{
  WINDOW *w_help;

  w_help   = wn_open(_DOUBLE_LINE, 5,5,MAXXSIZE-10,MAXYSIZE-10,_window_att,_window_att);
  wn_title(w_help,"PACKMON Help Window");

  wn_printf(w_help,"\n\n\n\n             This could be help text \n");
  getch();
  wn_close(w_help);
}

