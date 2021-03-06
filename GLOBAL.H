#ifndef _GLOBAL_H
#define	_GLOBAL_H

/* Global definitions used by every source file.
 * Some may be compiler dependent.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if	defined(__TURBOC__) || defined(__STDC__) || defined(LATTICE)
#define	ANSIPROTO	1
#endif

/* Distinguish between Turbo C 2.0, and Borland C++ 2.0,3.0,3.1 */
#if defined(__TURBOC__) && defined(__BORLANDC__)
#define DFAR far
#else
#define DFAR
#endif

#ifndef	__ARGS
#ifdef	ANSIPROTO
#define	__ARGS(x)	x
#else
#define	__ARGS(x)	()
#endif
#endif


#if	!defined(AMIGA) && (defined(LATTICE) || defined(MAC) || defined(__TURBOC__))
/* These compilers require special open modes when reading binary files.
 *
 * "The single most brilliant design decision in all of UNIX was the
 * choice of a SINGLE character as the end-of-line indicator" -- M. O'Dell
 *
 * "Whoever picked the end-of-line conventions for MS-DOS and the Macintosh
 * should be shot!" -- P. Karn's corollary to O'Dells' declaration
 */
#define	READ_BINARY	"rb"
#define	WRITE_BINARY	"wb"
#define	APPEND_BINARY	"ab+"
#define	READ_TEXT	"rt"
#define	WRITE_TEXT	"wt"
#define	APPEND_TEXT	"at+"
#define UPDATE_TEXT	"rt+"
#define CREATE_TEXT	"wt+"

#else

#define	READ_BINARY	"r"
#define	WRITE_BINARY	"w"
#define	APPEND_BINARY	"a+"
#define	READ_TEXT	"r"
#define	WRITE_TEXT	"w"
#define	APPEND_TEXT	"a+"
#define UPDATE_TEXT	"r+"
#define CREATE_TEXT	"w+"

#endif

/* These two lines assume that your compiler's longs are 32 bits and
 * shorts are 16 bits. It is already assumed that chars are 8 bits,
 * but it doesn't matter if they're signed or unsigned.
 */
typedef long int32;		/* 32-bit signed integer */
typedef unsigned short int16;	/* 16-bit unsigned integer */
typedef unsigned char byte_t;	/*  8-bit unsigned integer */
#define	uchar(x) ((unsigned char)(x))
#define	MAXINT16 65535		/* Largest 16-bit integer */
#define	MAXINT32 4294967295L	/* Largest 32-bit integer */
#define	NBBY	8		/* 8 bits/byte */

#define	HASHMOD	7		/* Modulus used by hash_ip() function */

/* The "interrupt" keyword is non-standard, so make it configurable */
#if	defined(__TURBOC__) && defined(MSDOS)
#define	INTERRUPT	void interrupt
#else
#define	INTERRUPT	void
#endif

/* Note that these definitions are on by default if none of the Turbo-C style
 * memory model definitions are on; this avoids having to change them when
 * porting to 68K environments.
 */
#if	!defined(__TINY__) && !defined(__SMALL__) && !defined(__MEDIUM__)
#define	LARGEDATA	1
#endif

#if	!defined(__TINY__) && !defined(__SMALL__) && !defined(__COMPACT__)
#define	LARGECODE	1
#endif

/* Since not all compilers support structure assignment, the ASSIGN()
 * macro is used. This controls how it's actually implemented.
 */
#ifdef	NOSTRUCTASSIGN	/* Version for old compilers that don't support it */
#define	ASSIGN(a,b)	memcpy((char *)&(a),(char *)&(b),sizeof(b));
#else			/* Version for compilers that do */
#define	ASSIGN(a,b)	((a) = (b))
#endif

/* Define null object pointer in case stdio.h isn't included */
#ifndef	NULL
/* General purpose NULL pointer */
#define	NULL (void *)0
#endif
#define	NULLCHAR (char *)0	/* Null character pointer */
#define	NULLCHARP (char **)0	/* Null character pointer pointer */
#define	NULLINT	(int *)0	/* Null integer pointer */
#define	NULLFP	 (int (*)())0	/* Null pointer to function returning int */
#define	NULLVFP	 (void (*)())0	/* Null pointer to function returning void */
#define	NULLVIFP (INTERRUPT (*)())0
#define	NULLFILE (FILE *)0	/* Null file pointer */

/* standard boolean constants */
#define FALSE 0
#define TRUE 1
#define NO 0
#define YES 1

#define CTLA 0x1
#define CTLB 0x2
#define CTLC 0x3
#define CTLD 0x4
#define CTLE 0x5
#define CTLF 0x6
#define CTLG 0x7
#define CTLH 0x8
#define CTLI 0x9
#define CTLJ 0xa
#define CTLK 0xb
#define CTLL 0xc
#define CTLM 0xd
#define CTLN 0xe
#define CTLO 0xf
#define CTLP 0x10
#define CTLQ 0x11
#define CTLR 0x12
#define CTLS 0x13
#define CTLT 0x14
#define CTLU 0x15
#define CTLV 0x16
#define CTLW 0x17
#define CTLX 0x18
#define CTLY 0x19
#define CTLZ 0x1a

#define	BELL	CTLG
#define	BS	CTLH
#define	TAB	CTLI
#define	LF	CTLJ
#define	FF	CTLL
#define	CR	CTLM
#define	XON	CTLQ
#define	XOFF	CTLS
#define	ESC	0x1b
#define	DEL	0x7f

/* string equality shorthand */
#define STREQ(x,y) (strcmp(x,y) == 0)

/* Extract a short from a long */
#define	hiword(x)	((int16)((x) >> 16))
#define	loword(x)	((int16)(x))

/* Extract a byte from a short */
#define	hibyte(x)	((unsigned char)((x) >> 8))
#define	lobyte(x)	((unsigned char)(x))

/* Extract nibbles from a byte */
#define	hinibble(x)	(((x) >> 4) & 0xf)
#define	lonibble(x)	((x) & 0xf)

/* Various low-level and miscellaneous functions */
unsigned long availmem __ARGS((void));
void *callocw __ARGS((unsigned nelem,unsigned size));
int32 clock();
int dirps __ARGS((void));
int getopt __ARGS((int argc,char *argv[],char *opts));
int atoip __ARGS((char *));
int htoi __ARGS((char *));
long htol __ARGS((char *));
char *inbuf __ARGS((int16 port,char *buf,int16 cnt));
int16 hash_ip __ARGS((int32 addr));
int istate __ARGS((void));
void log __ARGS((int s,char *fmt, ...));
int log2 __ARGS((int16 x));
void *ltop __ARGS((long));
void *mallocw __ARGS((unsigned nb));
char *outbuf __ARGS((int16 port,char *buf,int16 cnt));
long ptol __ARGS((void *));
void restore __ARGS((int));
void rflush __ARGS((void));
void rip __ARGS((char *));
char *smsg __ARGS((char *msgs[],unsigned nmsgs,unsigned n));
int tprintf __ARGS((char *fmt,...));
#if	!defined __TURBOC__
char *strdup __ARGS((const char *));
#endif
int wildmat __ARGS((char *s,char *p,char **argv));

#ifdef TNOS_68K
#include "osk.h"
#endif

#ifdef	AZTEC
#define	rewind(fp)	fseek(fp,0L,0);
#endif

#if	defined(__TURBOC__) && defined(MSDOS)
#define movblock(so,ss,do,ds,c)	movedata(ss,so,ds,do,c)
#ifndef outportw
#define outportw outport
#endif
#ifndef inportw
#define inportw inport
#endif

#else

/* General purpose function macros already defined in turbo C */
#ifndef	min
#define	min(x,y)	((x)<(y)?(x):(y))	/* Lesser of two args */
#endif
#ifndef max
#define	max(x,y)	((x)>(y)?(x):(y))	/* Greater of two args */
#endif
#ifdef	MSDOS
#define MK_FP(seg,ofs)	((void far *) \
			(((unsigned long)(seg) << 16) | (unsigned)(ofs)))
#endif
#endif	/* __TURBOC __ */

#ifdef	AMIGA
/* super kludge de WA3YMH */
#ifndef	fileno
#include <stdio.h>
#endif
#define fclose(fp)	amiga_fclose(fp)
extern int amiga_fclose __ARGS((FILE *));
extern FILE *tmpfile __ARGS((void));

extern char *sys_errlist[];
extern int errno;
#endif

/* Externals used by getopt */
extern int optind;
extern char *optarg;

/* Threshold setting on available memory */
extern int32 Memthresh;

/* System clock - count of ticks since startup */
extern int32 Clock;

/* Various useful standard error messages */
extern char Badhost[];
extern char Badinterface[];
extern char Existingiface[];
extern char Nospace[];
extern char Notval[];
extern char Version[];
extern char Nosversion[];
extern char *Hostname;

/* Your system's end-of-line convention */
extern char Eol[];

/* Your system OS - set in files.c */
extern char System[];

extern void (*Gcollect[])();

/* I know this is cheating, but it definitely makes sure that
 * each module has config.h included ! - WG7J
 */
/*
#ifndef _CONFIG_H
#include "config.h"
#endif
*/

#endif	/* _GLOBAL_H */
