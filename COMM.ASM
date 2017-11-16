;*#+File*********************************************************
;
; FILE: COMM.ASM
;
; DESC: COM routines for HvA packet monitor
;       This file contains the routines for initializing COM
;       registers and interrupt vectors, and de_installing them
;
; PROC: Initialize_COM
;       Restore_COM
;
; HIST: 940118 V0.1 - Initial version
;
;*#-File*********************************************************


;*** EQUATIONS


Dosseg                        ;Use standard segmentation
.Model Large,C

.Data

;--------------------
; Primary parameters
;--------------------
packet_int_no   db 60h  ;software interrupt
com_irq         db 3    ;COM port IRQ
com_base        dw 2f8h ;COM port base
bit_rate        dw 1200 ;bps
tx_head         dw 480  ;transmition header length in bits
tx_tail         dw 24   ;transmition tail length in bits
slot_time       db 120  ;slot time for carrier sensing in bits
persistance     db 64   ;p-persistence
carrier_sense   db 2    ;carrier sensing
                        ;0 - don't care - just transmit (full duplex)
                        ;1 - sense DCD line
                        ;2 - sense data transitions (BAYCOM-like)
                        ;3 - deliver from data analysis

PUBLIC packet_int_no
PUBLIC com_irq
PUBLIC com_base
PUBLIC bit_rate
PUBLIC tx_head
PUBLIC tx_tail
PUBLIC slot_time
PUBLIC persistance
PUBLIC carrier_sense


;------------------------------------------------
;secondary parameters computed from primary ones
;------------------------------------------------
irq_mask        db 0    ;IRQ mask for 8259
cl_bit_len      dw 0    ;bit len in system clock (8253/4) ticks
cl_bit_len_2    dw 0    ;half bit len in clock ticks
bd_slot_time    dw 0    ;slot time in baud generator ticks / 8
bd_bit_len      db 0    ;bit len in baud generator ticks / 8

PUBLIC irq_mask
PUBLIC cl_bit_len
PUBLIC cl_bit_len_2
PUBLIC bd_slot_time
PUBLIC bd_bit_len


;**** Local variables

even
save_IER    db 0
save_LCR    db 0
save_MCR    db 0
save_DLL    db 0
save_DLM    db 0
save_irq_en db 0FFh ;save irq enabled in 8259

Old_COM_ISR EQU THIS DWORD
Old_COM_ISR_seg  DW 0	; segment address and offset address
Old_COM_ISR_off  DW 0	;



;**** External procedures

Extrn COM_ISR      : PROC

;**** CODE *************************************************************

.Code

PAGE

;--#+func----------------------------------------------------------------
;    FUNCTION: initialize_COM
;     PURPOSE:
;      SYNTAX:
; DESCRIPTION: Save old COM ISR
;              Set new COM ISR Vector
;     RETURNS:
;     HISTORY:
;--#-func----------------------------------------------------------------
                        PUBLIC initialize_COM
initialize_COM          PROC FAR
    push ax
    push dx
    pushf               ;save CPU interrupt flag
    cli                 ;disable interrupts

    push  es				  ; Get old vector
    mov   al,com_irq
    add   al,8
    mov   ah,35h
    int   21h
    mov   dx,es
    pop   es

    mov   Old_COM_ISR_seg,dx		  ; Store old vector
    mov   Old_COM_ISR_off,bx

    push  ds
    mov   ax, SEG COM_ISR		  ; Set new vector
    mov   ds,ax
    mov   dx, OFFSET COM_ISR
    mov   al,com_irq
    add   al,8
    mov   ah,25h
    int   21h
    pop   ds

    ;----------------------
    ; Save old IRQ mask and
    ; set new one
    ;----------------------
    mov ah,irq_mask
    not ah
    in al,21h           ;read 8259 mask
    or al,ah            ;extract com irq mask
    mov save_irq_en,al  ;save it
    in al,21h           ;enable com irq in 8259
    and al,ah           ;by clearing the right bit.
    out 21h,al

    ;------------------
    ;save COM registers
    ;------------------
    mov dx,com_base     ;dx=com_base
    inc dx              ;dx=IER
    in al,dx            ;save IER
    mov save_IER,al
    xor al,al           ;disable all COM interrupts
    out dx,al

    add dx,2            ;dx=LCR
    in al,dx            ;save LCR
    mov save_LCR,al

    inc dx              ;dx=MCR
    in al,dx            ;save MCR
    mov save_MCR,al

    dec dx              ;dx=LCR
    mov al,81h
    out dx,al           ;enable divisor read/write

    sub dx,3            ;dx=com_base=DLL
    in al,dx            ;read DLL
    mov save_DLL,al     ;save it

    inc dx              ;dx=DLM
    in al,dx            ;read DLM
    mov save_DLM,al     ;save it

    dec dx              ;dx=com_base=DLL
    mov ax,bd_slot_time ;set rate divisor
    out dx,ax           ;to slot_time*bd_bit_len

    add dx,3            ;dx=LCR
    mov al,01h          ;set 6 data/1 stop/no parity format
    out dx,al

    inc dx              ;dx=MCR
    mov al,09h          ;set DTR high, RTS low.
    out dx,al           ;and OUT2 high

    sub dx,3            ;dx=IER
    mov al,0Ah
    out dx,al           ;enable TxEmpty and modem status interrupt

    add dx,4            ;dx=LSR
    in al,dx            ;read LSR ;to clear possible line status int.

    inc dx
    in al,dx            ;read MSR to clear possible modem status int

    sub dx,6            ;dx=com_base again
    in al,dx            ;read Rx buffer
    in al,dx            ;to clear any possible pending Rx int.

    mov al,000000b      ;load Tx with 000000 char
    out dx,al
    out dx,al

    ;------------------------
    ; restore interrupt flag
    ;------------------------
    popf
    pop dx
    pop ax
    ret

initialize_COM ENDP

;--#+func----------------------------------------------------------------
;    FUNCTION: restore_COM
;     PURPOSE:
;      SYNTAX:
; DESCRIPTION:
;     RETURNS:
;     HISTORY:
;--#-func----------------------------------------------------------------
restore_COM         PROC FAR
    pushf
    push ax
    push dx
    cli

    ;--------------------------
    ; Restore interrupt vector
    ;--------------------------
    push ds
    mov dx, Old_COM_ISR_off
    mov ax, Old_COM_ISR_seg
    mov ds, ax
    mov al,com_irq
    add al,8
    mov ah,25h
    int 21h
    pop ds

    ;--------------------
    ; Restore IRQ mask
    ;--------------------
    in al,21h
    or al,irq_mask
    and al,save_irq_en
    out 21h,al

    ;-----------------------
    ; Restore COM registers
    ;-----------------------
    mov dx,com_base
    in al,dx        ;read Rx to clear a possible int.
    in al,dx
    inc dx          ;dx=IER
    mov al,save_IER ;restore IER
    out dx,al
    add dx,2        ;dx=LCR
    mov al,save_LCR ;restore LCR
    out dx,al
    inc dx          ;dx=MCR
    mov al,save_MCR ;restore MCR
    out dx,al
    inc dx          ;dx=LSR
    in al,dx        ;read LSR to clear possible int.
    inc dx          ;dx=MSR
    in al,dx        ;read MSR to clear possible int.
    sub dx,3        ;dx=LCR
    in al,dx        ;enable rate divisor access
    or al,80h
    out dx,al
    sub dx,3        ;dx=com_base=DLL
    mov al,save_DLL ;restore rate divisor
    out dx,al
    inc dx          ;dx=DLM
    mov al,save_DLM
    out dx,al
    add dx,2        ;dx=LCR
    mov al,save_LCR ;restore LCR again
    out dx,al

    pop dx
    pop ax
    popf
    ret

restore_COM ENDP

END




