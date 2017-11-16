/**#+FILE****************************************************************

    FILE: CONFIG.H
    DESC: HEADER FILE FOR CONFIG.C
    HIST: 940328 V0.1

***#-FILE****************************************************************/

#include <window.h>

/*-----/ Prototypes /-----*/

void configure(void);

/*-----/ Make variables external /-----*/

extern char gr_line[];
extern int  hex_debug;
extern int  disklog;
extern int  debuglevel;

extern unsigned int prev_timer_count;

extern WINDOW *w_mon;
extern WINDOW *w_stat;
extern WINDOW *w_heard;
