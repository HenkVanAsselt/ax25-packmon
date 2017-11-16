/****************************************
  FILE : MISC.H
  DESC : Header file for MISC.C
  DATE : 940311 V0.1
*****************************************/

#ifndef _TIME_T_DEFINED
#include <time.h>
#endif

char *timestr(time_t t);
void debug(char *format, ...);
char *smsg(char **msgs, unsigned int nmsgs, unsigned int n);
void dump_hex(unsigned char *d, int len);
char *get_frametype_str(int frametype);

