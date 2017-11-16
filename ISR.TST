PAGE 72,132
TITLE HvA Packet Monitor Interrupt

;*#+File*********************************************************
;
; FILE: ISR.ASM
;
; DESC: Interrupt service routines for HvA packet monitor
;
; HIST: 940118 V0.1 - Initial version
;
;*#-File*********************************************************

Dosseg                        ;Use standard segmentation
.Model Large,C
.Data

INCLUDE ISR.INC

iir         db 0
Upcall      db 0
Pos         dw 320
Framelen    dw 0

RxFrameValid DB 0

PUBLIC Upcall

databuf db MAXFRAMELEN+2 dup(0)
PUBLIC databuf

.XLIST
INCLUDE crc.inc     ; CRC Table
.LIST
.SALL

DEBUG MACRO char
        push ax
        mov  al,char
        call FAR PTR inp_debug
        pop ax
      ENDM


PAGE
.Code
;--#+func----------------------------------------------------------------
;    FUNCTION: CRCPass
;     PURPOSE: Calculate CRC from table
;      SYNTAX: in: dx = partial CRC
;                  ah = char to process
; DESCRIPTION: CRC in DX must be initialized with 0FFFFh
;              and inverted after passing through all characters
;     RETURNS:
;     HISTORY:
;--#-func----------------------------------------------------------------
CRCPass         PROC FAR
    push bx
    push ax
    xor dl,ah
    mov bl,dl
    xor bh,bh
    add bx,bx
    mov dl,dh
    xor dh,dh
    xor dx,[CRCtable+bx]
    pop ax
    pop bx
    ret

CRCPass ENDP

PAGE
;--#+func----------------------------------------------------------------
;    FUNCTION: COM_ISR
;     PURPOSE: COM interrupt service routine
;      SYNTAX:
; DESCRIPTION:
;     RETURNS:
;     HISTORY:
;--#-func----------------------------------------------------------------
                        PUBLIC COM_ISR
COM_ISR                 PROC FAR

    ;--------------------
    ; Save registers
    ;--------------------
    push ax
    push bx
    push cx
    push dx
    push ds
    push si

    mov ax, @data       ; Initialize ds
    mov ds, ax

    ;----------------------------------
    ; Check if COM interrupt is pending
    ;----------------------------------
	mov dx,WORD PTR com_base    ;set UART base
    add dx,MSR                  ;Get modem status register
    in  al,dx                   ;
    sub dx,MSR                  ;Set dx back to UART base
    and al,1                    ;Did CTS change state?
    jnz get_timer               ;Yes
    jmp End_ISR                 ;No

get_timer:
    mov     al,0                     ; Get timer count in ax
    out     43h,al
    in      al,40h
    mov     ah,al
    in      al,40h
    xchg    al,ah
    mov     dx,WORD PTR prev_timer_count   ; Get previous timer count
    sub     dx,ax                    ; Calculate difference (countdown!)
    mov     WORD PTR prev_timer_count,ax   ; Save actual timer count
    mov     ax,994                  ; Get bitlen in timerticks
    mov     bx,ax                   ; to bx
    shr     ax,1                    ; ax = bitlen in timerticks / 2
    cmp     dx,ax                   ; timediff < bitlen/2 ?
    jb      jmp_End_ISR             ; Yes
    add     ax,bx                   ; No, samplelength(ax) <-- bitlen * 1.5
    xor     cx,cx                   ; Clear no# periods counter

calc_no_periods:                    ; CALCULATE NUMBER OF PERIODS BETWEEN TWO '0's
    cmp     dx,ax                   ; Compare timediff with samplelength
    jb      check_no_periods        ; Jump if below
    add     ax,bx                   ; samplelength += bitlen
    inc     cx                      ; Inc. incoming periods counter
    jmp     short calc_no_periods
;    cmp     cl,7                    ; no# periods <= 7 ?
;    jbe     calc_no_periods         ; Yes

check_no_periods:                   ; CHECK NUMBER OF MEASURED PERIODS
    cmp     cl,6                    ; No# periods < 6?
    jb      store_bits              ; Yes, store cl-1 '1' bits and a '0'
    cmp     cl,7                    ; HDLC flag? (01111110)
    je      RxFoundFlag             ; Yes, we found a flag
    jmp     short Invalid_Frame     ; No, this is an invalid frame

RxFoundFlag:                        ; FLAG FOUND
;    DEBUG   'F'
    cmp     BYTE PTR RxFrameValid,1 ; Start Flag found ?
    jne     reset_vars              ; No, this is the start flag

CheckFrameLen:                      ; END FLAG FOUND. CHECK FRAME LENGTH
;    DEBUG   'E'
    mov     cx, WORD PTR Framelen   ; Yes, load framelength
    cmp     cx, 4                   ; Check framelength  (WAS 10 !!!)
    jb      Invalid_Frame           ; Frame to short, reset all
    cmp     cx,14Ah                 ; Framelen > 330 octets ?
    ja      Invalid_Frame           ; Yes, reset all
    call    FAR PTR save_databuffer ; Length OK, save it
    jmp     short End_ISR

jmp_End_ISR:
    jmp     short End_ISR


Invalid_Frame:                      ; SET INVALID FRAME
;    DEBUG   'I'
    mov     BYTE PTR RxFrameValid,0 ; Reset valid frame
    jmp     short End_ISR           ; End of interrupt routine

reset_vars:                         ; START FLAG FOUND, RESET VARIABLES
;    DEBUG   'S'
    mov     BYTE PTR shiftreg,0     ; Reset shift register
    mov     BYTE PTR bitcntr,8      ; Reset Bitcounter
    mov     WORD PTR RxReadPtr,0    ; Reset Received Read pointer
    mov     WORD PTR write_ptr,0    ; Reset Received write pointer
    mov     BYTE PTR RxFrameValid,1 ; Set Frame Valid flag
    mov     WORD PTR Framelen,0     ; Reset framelength
    jmp     short End_ISR           ; End of Interrupt Service Routine

store_bits:
    cmp     BYTE PTR RxFrameValid,1 ; Check if still valid frame
    jne     End_ISR                 ; No, goto end
    mov     dl,bitcntr              ; Get bitcounter of shiftregister
    mov     bx,WORD PTR write_ptr   ; Get writepointer of data buffer
    mov     al,shiftreg             ; Get contents of shiftregister

    jcxz    store_0_bit             ; Jump if cx == 0
    mov     ah,cl                   ; Save number of bits in ah

store_1_bits:                       ; Store cx '1' bits
    stc                             ; Set carry to indicate '1' bit
;    DEBUG '1'
    call    FAR PTR StoreBit        ; Store '1' bit
    loop    store_1_bits            ; Store in total cx 1 bits

    cmp     ah,5                    ; 5 '1' bits received?
    je      save_tmp_data           ; Yes, save data and return

store_0_bit:
;    DEBUG '0'
    clc                             ; Clear carry flag to indicate '0' bit
    call    FAR PTR StoreBit        ; Store '0' bit

save_tmp_data:
    mov     shiftreg,al             ; Save shift regiter
    mov     bitcntr,dl              ; Save bitcounter of shiftregister
    mov     WORD PTR write_ptr,bx   ; Save buffer write pointer

End_ISR:
    mov al,63h                      ;EOI
    out OCW2,al
    pop     si
    pop     ds
    pop     dx
    pop     cx
    pop     bx
    pop     ax
    iret                            ; Interrupt return

COM_ISR ENDP

;--#+func----------------------------------------------------------
;    FUCNTION: save_datbuffer
;     PURPOSE: Save received data in seperate buffer for main
;      SYNTAX:
; DESCRIPTION:
;     RETURNS: nothing
;     HISTORY: 940321 V0.1
;--#-func----------------------------------------------------------

save_databuffer PROC FAR

    mov     RxFrameLen,cx
    mov     databuf[0],cl           ; Store framelen in databuf
    mov     databuf[1],ch
    mov     bx,WORD PTR RxReadPtr   ; Initialize pointers
    mov     si,2
copy_octet:
    mov     al,BYTE PTR RxFrameData[bx]
    mov     BYTE PTR databuf[si],al
    inc     bx
    and     bx,3FFh                 ; wrap if needed (0...1023)
    inc     si
    loop    copy_octet
    inc     BYTE PTR Upcall         ; Increment Upcall flag

    mov     bx,WORD PTR write_ptr   ; Save current writeptr to
    mov     WORD PTR RxReadPtr,bx   ; RxReadPtr

save_databuffer ENDP

;--#+func----------------------------------------------------------------
;    FUNCTION: StoreBit
;     PURPOSE:
;      SYNTAX:
; DESCRIPTION:
;     RETURNS:
;     HISTORY: 940212 V0.1
;--#-func----------------------------------------------------------------
StoreBit   PROC    FAR

   rcr     al,1                          ; Rotate thru carry
   dec     dl
   jnz     ret_StoreBit                  ; Jump if not zero
   mov     dl,8
   mov     byte ptr RxFrameData[bx],al   ; (79E2:0972=75h)
   inc     WORD PTR Framelen
   inc     bx
   and     bx,3FFh                       ; (0...1023)

   push  dx
   mov   dl,al
   shr   dl,1
   DEBUG dl
   pop   dx


ret_StoreBit:
   ret

StoreBit ENDP

PAGE
;--#+func----------------------------------------------------------------
;    FUNCTION: Timer_ISR
;     PURPOSE: Timer Interrupt Service Routine (Vector 8)
;      SYNTAX:
; DESCRIPTION:
;     RETURNS:
;     HISTORY:
;--#-func----------------------------------------------------------------
                                PUBLIC Timer_ISR
Timer_ISR                       PROC FAR

    ;sti                        ;shall we enable interrupts already here ?

    PUSH_ALL                    ; Save registers

    mov  ax,@data               ; Initialize ds
    mov  ds,ax

    mov al,BYTE PTR TimerISRActive       ;check for possible recursive call
    and al,al
    jnz TimerISR_Already_Active ;jump if it is so

    mov al,0FFh                 ;mark Timer ISR as really active
    mov BYTE PTR TimerISRActive,al
    sti                         ;enable interrupts so Tx and Rx can run
                                ;while we process data collected by Rx
    ;--------------------------
    ; Code comes here
    ;--------------------------


TimerISR_end:
    xor al,al
    mov BYTE PTR TimerISRActive,al

TimerISR_Already_Active:

    POP_ALL                     ; Restore registers
    iret

Timer_ISR ENDP

PAGE
;--#+func----------------------------------------------------------------
;    FUNCTION: Initialize_Timer
;     PURPOSE: Save current timer ISR vector
;              set vector to our 'Timer_ISR' routine
;      SYNTAX:
; DESCRIPTION:
;     RETURNS:
;     HISTORY:
;--#-func----------------------------------------------------------------
                   PUBLIC Initialize_Timer
Initialize_Timer   PROC FAR

    ;--------------------
    ; Initialize
    ;--------------------
    pushf
    push ax
    push dx
    cli

    push  es                  ; Get old vector
    mov   ax,351Ch
    int   21h
    mov   dx,es
    mov   ax,bx
    pop   es

    mov   Old_Timer_seg,dx        ; Store old vector
    mov   Old_Timer_off,ax

    push  ds
    mov   ax, SEG Timer_ISR       ; Set new vector
    mov   ds,ax
    mov   dx, OFFSET Timer_ISR
    mov   ax,251Ch
    int   21h
    pop   ds

    ;--------------------
    ; Return from routine
    ;--------------------
    pop dx
    pop ax
    popf
    ret

Initialize_Timer ENDP

PAGE
;--#+func----------------------------------------------------------------
;    FUNCTION: Restore_timer
;     PURPOSE: Restore vector to original timer ISR
;      SYNTAX:
; DESCRIPTION:
;     RETURNS:
;     HISTORY:
;--#-func----------------------------------------------------------------
                PUBLIC Restore_Timer
Restore_Timer   PROC FAR

    ;--------------------
    ; Initialize
    ;--------------------
    pushf
    push ax
    push dx
    cli

    push  ds                ; Restore vector 8h (timer handling)
    mov   dx, Old_Timer_off
    mov   ax, Old_Timer_seg
    mov   ds, ax
    mov   ax, 251Ch
    int   21h
    pop   ds

    ;---------------------
    ; Return from routine
    ;---------------------
    pop dx
    pop ax
    popf
    sti
    ret

Restore_Timer ENDP

PAGE
;--#+func----------------------------------------------------------------
;    FUNCTION: initialize_COM
;     PURPOSE:
;      SYNTAX:
; DESCRIPTION: Save old COM ISR
;              Set new COM ISR Vector
;              Only COM2 at the moment
;     RETURNS:
;     HISTORY:
;--#-func----------------------------------------------------------------
                        PUBLIC initialize_COM
initialize_COM          PROC FAR
    push ax
    push dx
    pushf               ;save CPU interrupt flag
    cli                 ;disable interrupts

    push  es
    mov   al,11         ; com_irq + 8
    mov   ah,35h
    int   21h
    mov   dx,es
    mov   ax,bx
    mov   Old_COM_ISR_seg,dx          ; Store old vector
    mov   Old_COM_ISR_off,ax
    pop   es

    push  ds
    mov   ax, SEG COM_ISR         ; Set new vector
    mov   ds,ax
    mov   dx, OFFSET COM_ISR
    mov   al,11                    ; com_irq + 3
    mov   ah,25h
    int   21h
    pop   ds

    mov     al,0C1h
    out     OCW2,al                 ; port 20h, 8259-1 int command
    in      al,OCW1                 ; port 21h, 8259-1 int IMR
    mov     ah,0FEh
    stc                             ; Set carry flag
    rcl     ah,cl                   ; Rotate thru carry
    and     al,ah
    out     OCW1,al                 ; port 21h, 8259-1 int comands



    in  al,OCW1                     ;enable com irq in 8259
    and al, not IRQ3_MASK           ;by clearing the right bit.
    out OCW1,al                     ;new interrupt mask in interrupt controller

    mov al,IRQ3_EOI                 ; IRQ3_EOI
    out OCW2,al                     ; OCW2

    mov     dx,WORD PTR com_base    ; SET UART BASE ADDRESS

    add     dx,MSR                  ; SET MODEM STATUS REGISTER
    in      al,dx
    sub     dx,MSR

    add     dx,MCR                  ; SET MODEM CONTROL REGISTER
    mov     al,9
    out     dx,al
    sub     dx,MCR

    add     dx,LCR                  ; SET LINE CONTROL REGISTER
    mov     al,43h
    out     dx,al
    sub     dx,LCR

    add     dx,IER                  ; SET INTERRUPT ENABLE REGISTER
    mov     al,8
    out     dx,al
    sub     dx,IER

   ;------------------------------
   ; restore registers and return
   ;------------------------------
   popf
   pop dx
   pop ax
   ret

initialize_COM ENDP

PAGE
;--#+func----------------------------------------------------------------
;    FUNCTION: restore_COM
;     PURPOSE:
;      SYNTAX:
; DESCRIPTION: Only COM2 at the moment
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
    mov al,11                    ; com_irq + 8
    mov ah,25h
    int 21h
    pop ds

    ;--------------------
    ; Restore IRQ mask
    ;--------------------
    in al,21h
    or al,IRQ3_MASK             ; irq_mask
    and al,save_irq_en
    out 21h,al

    ;-----------------------
    ; Restore COM registers
    ;-----------------------
;   mov dx,WORD PTR com_base

;   add dx,RBR
;   in al,dx        ;read Rx to clear a possible int.
;   in al,dx
;   sub dx,RBR

;   add dx,IER
;   mov al,save_IER ;restore IER
;   out dx,al
;   sub dx,IER

;   add dx,LCR      ;dx=LCR
;   mov al,save_LCR ;restore LCR
;   out dx,al
;   sub dx,LCR

;   add dx,MCR      ;dx=MCR
;   mov al,save_MCR ;restore MCR
;   or al, RTS
;   or al, OUT2
;   out dx,al
;   sub dx,MCR

;   add dx,LSR
;   inc dx          ;dx=LSR
;   in al,dx        ;read LSR to clear possible int.
;   sub dx,LSR

;   add dx,MSR
;   in al,dx        ;read MSR to clear possible int.
;   sub dx,MSR

;   add dx,LSR
;   in al,dx        ;enable rate divisor access
;   or al,80h
;   out dx,al
;   sub dx,LSR

;   add dx,DLL      ;dx=com_base=DLL
;   mov al,save_DLL ;restore rate divisor
;   out dx,al
;   sub dx,DLL

;   add dx,DLM
;   mov al,save_DLM
;   out dx,al
;   sub dx,DLM

;   add dx,LCR
;   mov al,save_LCR ;restore LCR again
;   out dx,al
;   sub dx,LCR

    pop dx
    pop ax
    popf
    ret

restore_COM ENDP

;--#+func----------------------------------------------------------------
;    FUNCTION: inp_debug
;     PURPOSE:
;      SYNTAX:
; DESCRIPTION: Called by macro DEBUG.
;              This macro will push ax on the stack and put a character
;              in al
;     RETURNS:
;     HISTORY:
;--#-func----------------------------------------------------------------
inp_debug PROC FAR

        push cx                     ; Save registers
        push es
        push bx

        mov  cl,al                  ; Copy character to display

        inc  WORD PTR Pos           ; Increment position
        mov  bx, WORD PTR Pos       ; Get position
        cmp  bx,1280
        jl   pos_ok
        mov  WORD PTR Pos, 320

pos_ok:
        sub  bx,1                   ; Adjust for 0 offset
        shl  bx,1                   ; Adjust for word offset

        mov  ax,0B000h              ; Initialize segment register
        mov  es,ax                  ; with segment of screen memory
        mov  es:[bx],cl             ; Copy character to screen

        pop  bx
        pop  es                     ; Restore registers
        pop  cx
        ret

inp_debug ENDP

END
