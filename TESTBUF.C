#include <stdio.h>
#include <memory.h>
#include <string.h>
#include "buf.h"
#include "misc.h"


MBUF buffer;

unsigned char data[200];

void main(void);

void main()
{
  int i;
  int8 j;

  for (i=0 ; i<100 ; i++)
    data[i] = (unsigned char) i;



  buffer.len = 100;
  memcpy(buffer.data,data,100);
  setbufptr(&buffer,0L);

  printf("count = %d    len = %d    ptr = %p \n",buffer.cnt,buffer.len,buffer.dptr);

  j = pull8(&buffer);
  printf("j = %x, count = %d    len = %d    ptr = %p \n",j,buffer.cnt,buffer.len,buffer.dptr);

  j = pull8(&buffer);
  printf("j = %x, count = %d    len = %d    ptr = %p \n",j,buffer.cnt,buffer.len,buffer.dptr);

  j = pull8(&buffer);
  printf("j = %x, count = %d    len = %d    ptr = %p \n",j,buffer.cnt,buffer.len,buffer.dptr);

  j = pullchar(&buffer);
  printf("j = %x, count = %d    len = %d    ptr = %p \n",j,buffer.cnt,buffer.len,buffer.dptr);
  j = pullchar(&buffer);
  printf("j = %x, count = %d    len = %d    ptr = %p \n",j,buffer.cnt,buffer.len,buffer.dptr);
  j = pullchar(&buffer);
  printf("j = %x, count = %d    len = %d    ptr = %p \n",j,buffer.cnt,buffer.len,buffer.dptr);
  j = pullchar(&buffer);
  printf("j = %x, count = %d    len = %d    ptr = %p \n",j,buffer.cnt,buffer.len,buffer.dptr);

}


