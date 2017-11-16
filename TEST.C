#include <stdio.h>
#include <dos.h>

void main(void);

void main()
{
  void (interrupt far *int_handler)();
  long i;
  unsigned intno;
  unsigned char *p;
  union REGS regs;

  for (intno = 0xf0 ; intno < 0xFF ; intno++) {
    int_handler = _dos_getvect(intno);
    printf("intno %X : %p :",intno,int_handler);
    p = (char *)int_handler;
    for (i = 0 ; i<15 ; i++)
      printf("%02X ",(unsigned char)p[i]);
    printf("\n");
  }

  regs.h.ah = 0xfe;           /* Ask TFPCX version nr  */
  int86(0xfd,&regs,&regs);
  printf("TFPCX V%1d.%2d\n",regs.h.ah,regs.h.al);
  getch();


  regs.h.ah = 0x01;
  regs.h.al = 0x1b;
  int86(0xfd,&regs,&regs);

  regs.h.ah = 0x01;
  regs.h.al = 'M';
  int86(0xfd,&regs,&regs);

/*
  regs.h.ah = 0x01;
  regs.h.al = ' ';
  int86(0xfd,&regs,&regs);
*/

  regs.h.ah = 0x01;
  regs.h.al = 'U';
  int86(0xfd,&regs,&regs);

  regs.h.ah = 0x01;
  regs.h.al = 'I';
  int86(0xfd,&regs,&regs);

  regs.h.ah = 0x01;
  regs.h.al = 'S';
  int86(0xfd,&regs,&regs);

  regs.h.ah = 0x01;
  regs.h.al = 'C';
  int86(0xfd,&regs,&regs);

  regs.h.ah = 0x01;
  regs.h.al = 0x0d;
  int86(0xfd,&regs,&regs);

  for (i = 0 ; i<1000000 ; i++)  {
    regs.h.ah = 0x00;
    int86(0xfd,&regs,&regs);
    if (regs.h.ah == 0x01) {
      printf("%c",regs.h.al);
    }
  }

}


