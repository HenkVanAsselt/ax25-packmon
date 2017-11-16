######################################################
# Makefile for HvA Packet Monitor                    #
# 940118 V0.1                                        #
######################################################

CC =  cl /AL /L /W3 /Fs /c /Zi /DMEM_DEBUG
ASM = masm /DMSC /ML /W2 /Zi /Zd /L $*.asm,$*.obj,$*.lst,;
# LIB = lib window.lib -+$*.obj ,window.pub;
# ARJ = arj u packmon.arj

### Set inference rules
#########################

.c.obj:
    $(CC) $*.c
#    $(ARJ) $*.c

.asm.obj:
    $(ASM) $*
#    $(ARJ) $*.asm

### Set dependencies
######################

packmon.obj: $*.c $*.h ax25.h

isr.obj: $*.asm $*.inc

ax25.obj: $*.c ax25.h

ax25subr.obj: $*.c packmon.h ax25.h

ax25dump.obj: $*.c packmon.h ax25.h

log.obj: $*.c packmon.h config.h log.h

heard.obj: $*.c packmon.h heard.h config.h

buf.obj: $*.c buf.h

misc.obj: $*.c misc.h

netdump.obj: $*.c netrom.h

arpdump.obj: $*.c arp.h

iphdr.obj: $*.c ip.h

crc.obj: $*.c crc.h crctab.h

config.obj: $*.c config.h

status.obj: $*.c status.h

key.obj: $*.c

help.obj: $*.c

#################
# TEST PROGRAMS #
#################

packmon.exe: packmon.obj isr.obj \
             ax25.obj ax25subr.obj ax25dump.obj help.obj \
             iphdr.obj \
             log.obj heard.obj buf.obj misc.obj key.obj \
             arpdump.obj netdump.obj crc.obj config.obj status.obj \
             e:\lib\window.lib
  link @packmon.lnk

