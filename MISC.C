/*****************************************************************

  FILE : MISC.C
  DESC : Miscellaneuou packmon routines
  HIST : 940211 V0.1

******************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <ctype.h>
#include <stdarg.h>
#include <time.h>
#include "misc.h"
#include "config.h"
#include "ax25.h"
#include "log.h"

#define FALSE 0
#define TRUE !FALSE

/*-#+func----------------------------------------------------------
    FUNCTION: get_frametype_str()
     PURPOSE: Get frametype in human readable form
      SYNTAX: char *get_frametype_str(int frametype);
 DESCRIPTION: int frametype:    frametype
     RETURNS: Pointer to string indicating frametype
     HISTORY: 940701 V0.1
--#-func----------------------------------------------------------*/
char *get_frametype_str(int frametype)
{
  static char *str[] = {"I","RR","RNR","REJ","SABM","DISC","DM","UA","FRMR","UI"};

  switch(frametype) {
    case I:     return(str[0]); break;
    case RR:    return(str[1]); break;
    case RNR:   return(str[2]); break;
    case REJ:   return(str[3]); break;
    case SABM:  return(str[4]); break;
    case DISC:  return(str[5]); break;
    case DM:    return(str[6]); break;
    case UA:    return(str[7]); break;
    case FRMR:  return(str[8]); break;
    case UI:    return(str[9]); break;
  }
  return("");    /* Frametype not valid, empty string */
}

/*-#+func----------------------------------------------------------
    FUNCTION: timestr()
     PURPOSE: Given a time in seconds since Jan 1, 1970
      SYNTAX: char *timestr(time_t t)
 DESCRIPTION:
     RETURNS:
     HISTORY:
--#-func----------------------------------------------------------*/
char *timestr(time_t t)
{
    static char     s[30];
    struct tm   *timeptr;

    timeptr = localtime(&t);
    sprintf(s,"%02d:%02d:%02d",
        timeptr->tm_hour, timeptr->tm_min, timeptr->tm_sec);

    return s;
}

/*-#+func----------------------------------------------------------
    FUNCTION: debug()
     PURPOSE: Print debug information
      SYNTAX: void debug(char *format, ...);
 DESCRIPTION: prints debug information.
              Will add a CR/LF if this is not at end of string
     RETURNS: nothing
     HISTORY: 940306 V0.1
              940621 V0.1 - Added CR/LF detection
--#-func----------------------------------------------------------*/
void debug(char *format, ...)
{
  va_list arguments;
  char s[1024];
  FILE *f;
  int i;
  int crlf = FALSE;            /* Flag if CR/LF has been detected */

  if (getenv("OX"))
    f = stdaux;
  else
    f = stderr;

  /*-----------------------
    Build and print string
  ------------------------*/
  va_start(arguments,format);
  vsprintf(s,format,arguments);
  va_end(arguments);
  i = 0;
  while (s[i]) {
    fputc(s[i],f);
    if (s[i] == LF && s[i-1] != CR)
    {
      fputc(CR,f);
      crlf = TRUE;
    }
    if (s[i] == CR && s[i-1] != LF)
    {
      fputc(LF,f);
      crlf = TRUE;
    }
    i++;
  }

  /* Print CR/LF if not already done */
  if (!crlf) {
    fputc(CR,f);
    fputc(LF,f);
  }
}

/*-#+func----------------------------------------------------------
    FUNCTION: smsg()
     PURPOSE: Select from an array of strings,
              or return ascii number if out of range
      SYNTAX: char *smsg(char *msgs[], unsigned nmsgs, unsinged n) ;
 DESCRIPTION:
     RETURNS:
     HISTORY:
--#-func----------------------------------------------------------*/
char *smsg(char **msgs, unsigned int nmsgs, unsigned int n)
{
    static char buf[16];

    if(n < nmsgs && msgs[n] != '\0')
      return msgs[n];
    sprintf(buf,"%u",n);
    return buf;
}

/*-#+func----------------------------------------------------------
    FUNCTION: ctoh()
     PURPOSE: Convert byte into two-byte hex-ascii
      SYNTAX: void ctoh(char *s, char c);
 DESCRIPTION:
     RETURNS: nothing
     HISTORY: 940411 V0.1
--#-func----------------------------------------------------------*/
void ctoh(char *s, char c)
{
    static char convert[] = "0123456789abcdef";

    *s++ = convert[(c >> 4)&0xf];
    *s = convert[c&0xf];
}

/*-#+func----------------------------------------------------------
    FUNCTION: dump_hex()
     PURPOSE: Dump data fields of packet in hex, shifted ascii and straight ascii
      SYNTAX: dump_hex(unsigned char *d, int len);
 DESCRIPTION: unsigned char *d = pointer to array of data
              int len          = length of data array to dump
     RETURNS: pointer to decoded text
     HISTORY: 940411 V0.1
--#-func----------------------------------------------------------*/
void dump_hex(unsigned char *d, int len)
{
  int i,cnt;
  char outbuf[132];
  char *hp,*sp,*rp,sc;
  unsigned char c;
  static char *header = "\n  0 1 2 3 4 5 6 7 8 9 a b c d e f  0123456789abcdef    0123456789abcdef\n";

  debug(header);
  if (disklog)
    log(header);

  cnt = len;
  while(cnt != 0){
    /* Set starting columns in the output line for each field */
    hp = &outbuf[1];    /* hex */
    sp = &outbuf[35];   /* shifted ascii */
    rp = &outbuf[55];   /* regular ascii */

    memset(outbuf,' ',132);
    strcpy(&outbuf[73],"\n");

    for(i = 0;i<16 && cnt != 0;i++){
      c = *d++;  /* get char from frame */
      cnt--;
      ctoh(hp,c); /* hex display */
      hp += 2;
      sc = (c >> 1) & 0x7f;   /* shifted ascii */
      *sp++ = sc >= 0x20 ? sc : '.';
      c &= 0x7f;  /* regular ascii */
      *rp++ = c >= 0x20 ? c : '.';
    }
    /* Send the completed line */
    debug(outbuf);

    if (disklog)
      log(outbuf);
  }
}




