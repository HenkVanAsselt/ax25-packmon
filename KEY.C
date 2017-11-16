#include <conio.h>
#include <stdio.h>
#include <ctype.h>
#include <window.h>    /* For function key handling */

#include <mshell.h>

#include "config.h"
#include "packmon.h"
#include "ax25.h"
#include "heard.h"
#include "status.h"

/*-#+func----------------------------------------------------------
    FUNCTION: handle_key()
     PURPOSE: Handle keyboard input
      SYNTAX: int handle_key(void);
 DESCRIPTION: Checks if a key is available in the input buffer
              If so, the key will be read and processed.
     RETURNS: key read from keyboard buffer
     HISTORY: 940228 V0.1
--#-func----------------------------------------------------------*/
int handle_key(void)
{
  int c = '\0';

  if (kbhit()) {
    c = waitkey();
    switch(c) {
      case 'H':
      case 'h': DoHeard(); break;
      case 'S':
      case 's': DumpStatus(); break;
      case 'D':
      case 'd': hex_debug = !hex_debug; break;
      case 'L':
      case 'l': disklog = !disklog; break;
      case F1 : DoHelp(); break;
      case F2 : Mem_Display(stdaux);
      default : break;
    }
  }

  return(c);
}


