/**#+FILE****************************************************************

    FILE: CONFIG.C
    DESC: GLOBAL CONFIGURATION OF PACKMON.H
    HIST: 940328 V0.1

***#-FILE****************************************************************/

/*-----/ Include files /-----*/
#include <stdio.h>
#include <string.h>
#include <window.h>

/*-----/ Global variables /-----*/

char gr_line[81];        /* Graphical line */
int  hex_debug = FALSE;  /* Flag to dump data in hex format */
int  disklog   = TRUE;   /* Flag disklogging on/off         */
int  debuglevel = 3;

WINDOW *w_mon  = NULL;
WINDOW *w_stat = NULL;
WINDOW *w_heard = NULL;

/*-----/ Prototypes /-----*/

void configure(void);
void close_windows(void);

/*-#+func----------------------------------------------------------
    FUNCTION: configure()
     PURPOSE: Setup global variables
      SYNTAX: void configure(void);
 DESCRIPTION:
     RETURNS: nothing
     HISTORY: 940328 V0.1
--#-func---------------------------------------------------------*/
void configure()
{
  /*-----/ Setup graphical line /-----*/
  memset(gr_line,'Ä',80);
  gr_line[80] = '\0';


  /*-----/ Open windows /-----*/
  w_mon   = wn_open(_DOUBLE_LINE, 1,0,MAXXSIZE,(int)MAXYSIZE/2,_window_att,_window_att);
  wn_title(w_mon,"  Incoming data  ");
  w_heard = wn_open(_DOUBLE_LINE,(int)MAXYSIZE/2+1,0,(int)MAXXSIZE,(int)MAXYSIZE/2-1,_window_att,_window_att);
  wn_title(w_heard,"  Monitored Connections  ");
  w_stat  = wn_open(_NO_BORDER,0,0,MAXXSIZE,1,_window_att,_window_att);

  atexit(close_windows);
}


/*-#+func----------------------------------------------------------
    FUNCTION: close_windows()
     PURPOSE: close open windows
      SYNTAX: void close_windows(void);
 DESCRIPTION: Called by exit()
     RETURNS: nothing
     HISTORY: 940624 V0.1
--#-func----------------------------------------------------------*/
void close_windows()
{
  wn_close(w_mon);
  wn_close(w_heard);
  wn_close(w_stat);
}


