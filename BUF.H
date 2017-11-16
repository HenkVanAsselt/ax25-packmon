/*******************************************************
*
* FILE : BUF.H
* DESC : Header file for Buffer routines
* HIST : 940308 V0.1
*
*******************************************************/

#ifndef	_BUF_H
#define	_BUF_H

/*--- TYPEDEFS ---*/

typedef unsigned char int8;
typedef unsigned int  int16;
typedef unsigned long int32;

#define	uchar(x) ((unsigned char)(x))

typedef struct mbuf {
  int16 len;                 /* Actual number of bytes in buffer */
  int16 cnt;                 /* Number of bytes remaining        */
  unsigned char *dptr;       /* Actual buffer pointer            */
  unsigned char data[280];   /* 280 to be save                   */
}
MBUF;

/*--- PROTOTYPES ---*/

void setbufptr(MBUF *buf, unsigned long n);

int8 pull8(MBUF *buf);
int16 pull16(MBUF *buf);
int32 pull32(MBUF *buf);
unsigned char pullchar(MBUF *buf);
int16 pullup(MBUF *bph, unsigned char *buf, int16 cnt);

int8  get8(register char *buf);
int16 get16(register char *buf);
int32 get32(register char *buf);

void buf2data(unsigned char *dest, MBUF *src, int16 cnt);

#define	PULLCHAR(bpp)\
 ((bpp) != NULL && bpp->cnt > 1 ? \
 (bpp->cnt--,(unsigned char)bpp->data++) : pullchar(bpp))

#endif

