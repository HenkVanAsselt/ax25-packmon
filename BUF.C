/*******************************************************
*
* FILE : Buf.c
* DESC : Buffer routines
* HIST : 940308 V0.1
*
*******************************************************/

#include <memory.h>
#include "buf.h"

/*-#+func----------------------------------------------------------
    FUNCTION: setbufptr()
     PURPOSE: Set buffer pointer
      SYNTAX: void setbufptr(MBUF *buffer, unsigned long n) ;
 DESCRIPTION: Buffer uses 0-offset
     RETURNS:
     HISTORY:
    SEE ALSO: MBUF
--#-func----------------------------------------------------------*/
void setbufptr(MBUF *buffer, unsigned long n)
{
  buffer->dptr = buffer->data + n;
  buffer->cnt  = buffer->len - (int16) n;
}

/*-#+func----------------------------------------------------------
    FUNCTION: pull32()
     PURPOSE: read a int32 from buffer
      SYNTAX: int32 pull32(MBUF *buffer);
 DESCRIPTION:
     RETURNS:
     HISTORY:
    SEE ALSO: MBUF
--#-func----------------------------------------------------------*/
int32 pull32(MBUF *buf)
{
  int32 rval;

  rval = *(buf->dptr++);
  rval <<= 8;
  rval |= *(buf->dptr++);
  rval <<= 8;
  rval |= *(buf->dptr++);
  rval <<= 8;
  rval |= *(buf->dptr++);
  buf->cnt  -= 4;
  return(rval);
}
/*-#+func----------------------------------------------------------
    FUNCTION: pull16()
     PURPOSE:
      SYNTAX:
 DESCRIPTION:
     RETURNS:
     HISTORY:
--#-func----------------------------------------------------------*/
int16 pull16(MBUF *buf)
{
  int16 data;

  data = *(buf->dptr++);
  data <<= 8;
  data |= *(buf->dptr++);
  buf->cnt  -= 2;
  return(data);
}

/*-#+func----------------------------------------------------------
    FUNCTION: pull8()
     PURPOSE:
      SYNTAX:
 DESCRIPTION:
     RETURNS:
     HISTORY:
--#-func----------------------------------------------------------*/
int8 pull8(MBUF *buf)
{
  int8 data;

  data = (int8) *(buf->dptr);
  buf->dptr += 1;
  buf->cnt  -= 1;
  return(data);
}

/*-#+func----------------------------------------------------------
    FUNCTION: pullchar()
     PURPOSE: Pull single character from mbuf
      SYNTAX: int pullchar(MBUF *buf);
 DESCRIPTION:
     RETURNS: character, pulled from buffer
     HISTORY: 940703 V0.1 - Initial version
    SEE ALSO: MBUF
--#-func----------------------------------------------------------*/
unsigned char pullchar(MBUF *buf)
{
  unsigned char c;
  int16 n = 1;

  c = (unsigned char) *(buf->dptr);
  buf->dptr += n;
  buf->cnt  -= n;
  return(c);
}


/*-#+func----------------------------------------------------------
    FUNCTION: get16()
     PURPOSE: Get int16 variable from buffer
      SYNTAX: int16 get16(register char *cp);
 DESCRIPTION:
     RETURNS: value of int16 read from buffer
     HISTORY: 940627 V0.1
    SEE ALSO: MBUF
--#-func----------------------------------------------------------*/
int16 get16(register char *cp)
{
  register int16 x;

  x = uchar(*cp++);
  x <<= 8;
  x |= uchar(*cp);
  return x;
}

/*-#+func----------------------------------------------------------
    FUNCTION: get32()
     PURPOSE: Get int32 variable from buffer
      SYNTAX: int32 get32(register char *cp);
 DESCRIPTION:
     RETURNS: value of int32 read from buffer
     HISTORY: 940627 V0.1
--#-func----------------------------------------------------------*/
int32 get32(register char *cp)
{
  int32 rval;

  rval = uchar(*cp++);
  rval <<= 8;
  rval |= uchar(*cp++);
  rval <<= 8;
  rval |= uchar(*cp++);
  rval <<= 8;
  rval |= uchar(*cp);

  return rval;
}

/*-#+func----------------------------------------------------------
    FUNCTION: buf2data()
     PURPOSE: Copy a number of bytes to an array of char
      SYNTAX: void buf2data(unsigned char *dest, MBUF *src, int16 n)
 DESCRIPTION: unsigned char *dest = destination arrray
              MBUF *src           = source buffer
              int16 n             = number of bytes to copy
     RETURNS: nothing
     HISTORY: 930311 V0.2 - Added adjustment of 'cnt' (remaining)
    SEE ALSO: MBUF
--#-func----------------------------------------------------------*/
void buf2data(unsigned char *dest, MBUF *src, int16 n)
{
  memcpy(dest, src->dptr, n);
  src->dptr += n;
  src->cnt  -= n;
}

/*-#+func----------------------------------------------------------
    FUNCTION: pullup()
     PURPOSE: Copy and delete "cnt" bytes from beginning of packet.
      SYNTAX: int16 pullup(MBUF *bph, unsigned char *buf, int16 cnt);
 DESCRIPTION:
     RETURNS: Return number of bytes actually pulled off
     HISTORY:
    SEE ALSO: MBUF
--#-func----------------------------------------------------------*/
int16 pullup(MBUF *bph, unsigned char *buf, int16 cnt)
{
    register struct mbuf *bp;
    int16 n;
    int16 tot = 0;

    bp = bph;
    if (bp->cnt == 0)
      return(0);

    while(cnt != 0){
      n = cnt < bp->cnt ? cnt : bp->cnt;      /* Get minimum */
      if(buf != '\0'){
        if(n == 1)  /* Common case optimization */
          *buf = *bp->data;
        else {
          if(n > 1)
            /*
            memcpy(buf,bp->data,n);
            */
            memcpy(buf,bp->dptr,n);
        }
        buf += n;
      }
      tot += n;
      cnt -= n;
      bp->dptr += n;
      bp->cnt  -= n;
    }
    return tot;
}


