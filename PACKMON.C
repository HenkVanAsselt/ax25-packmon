#pragma linesize(132)
#pragma pagesize(72)
#pragma title("Hva Packet Monitor")

/*#+File*********************************************
*
* FILE: packmon.C
*
* DESC: Main program body of HvA Packet Monitor
*
* HIST:  Tue 940118 V0.1
*
**#-File*********************************************/

/*-------------------
  Include files
--------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <conio.h>      /* for kbhit() */
#include <ctype.h>
#include <time.h>
#include <window.h>
#include <hva_util.h>

#include <mshell.h>

#define MAINFILE
#include "config.h"
#include "packmon.h"
#include "ax25.h"
#include "misc.h"
#include "log.h"
#include "heard.h"
#include "status.h"

/* struct ax25_level1 *RXQueue; */ /* Receive queue */

/*---------------------
  Local prototypes
--------------------*/
void mainloop(void);

/*--------------------
  Program arguments
---------------------*/
ARG arg_tabel[] =
{
  { 'I',H_BYTE   ,&com_irq,       "COM IRQ number (2..7)             "},
  { 'B',H_WORD   ,&com_base,      "COM base address                  "},
};


/*-#+func----------------------------------------------------------
    FUNCTION: mainloop()
     PURPOSE: Perform the main loop
      SYNTAX: void mainloop(void);
 DESCRIPTION: -
     RETURNS: nothing
     HISTORY: 940508 V0.1
--#-func----------------------------------------------------------*/
void mainloop()
{
  int c;

  do {
    if (Upcall>0) {           /* Check if level1 packet arrived */
      Upcall--;                        /* Decrement Upcall flag */
	  AX25Level2( (struct ax25_level1 *) databuf);	  /* upcall */
    }
    DumpStatus();                           /* Show status line */
    c = handle_key();   /* Check if key pressed and react if so */
  }
  while (c != 'Q' && c != 'q' && c != ESC);
}


/*-#+func----------------------------------------------------------
    FUNCTION: main()
     PURPOSE: Main module of packmon.exe
      SYNTAX:
 DESCRIPTION:
     RETURNS:
     HISTORY: 940122 V0.1
--#-func----------------------------------------------------------*/
int main(int argc, char **argv)
{
  /*---------------------------------------
    Handle program arguments
  ---------------------------------------*/
  argc = getargs(argc,argv,arg_tabel,TABSIZE,NULL);
  if (argc == 0) {
    printf("\npackmon - %s (C) 1994 H.B.J. van Asselt\n\n",VERSION);
    pr_usage(arg_tabel,TABSIZE);
    printf("\n");
    exit(1);
  }

  wn_init();        /*-----/ Initialize HvA windows environment /-----*/
  configure();      /*-----/ Configure packmon /-----*/

/*
  printf("\n\n");
  printf("%s\n",gr_line);
  printf(" packmon - Packet Monitor %s by HvA\n",VERSION);
  printf(" com_base     %x\n",com_base      );
  printf(" \nKeys:\n");
  printf(" H = Show heard stations\n");
  printf(" S = Show status\n");
  printf(" Q = Quit\n");
  if (getenv("OX"))
    printf(" Debugging with OX is ON\n");
  printf("%s\n",gr_line);
  printf("\n");
*/

  LogInit();

  Initialize_COM();    /* Initialize UART and Interrupt Service Routines*/
  atexit(Restore_COM);

  HeardInit();         /* Initialize the HEARD list */

/*  Initialize_Timer(); */

  /*-----/ Initialize time of last receveived message /-----*/
  mainloop();

/*  Restore_Timer(); */

  exit(0);
  return(0);
}
