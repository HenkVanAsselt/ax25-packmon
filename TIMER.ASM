;*#+File*********************************************************
;
; FILE: TIMER.ASM
;
; DESC: Timer routines for HvA packet monitor
;       This file initializes Timer and restores timer interrupt
;       vector
;
; PROC: Initialize_Timer
;       Restore_Timer
;
; HIST: 940118 V0.1 - Initial version
;
;*#-File*********************************************************


;*** EQUATIONS


Dosseg                        ;Use standard segmentation
.Model Large,C

.Data

even

OldTimerISR EQU THIS DWORD
Old_Timer_seg  DW 0	; segment address
Old_Timer_off  DW 0	; offset address

RxBufferLen equ 1023                ;in words, must be 2^n-1
RxBuffer   dw RxBufferLen+1 dup(0)  ;Rx buffer storing periods between
                                    ;CTS transitions
                                    ;this buffer is filled by CTS transitions
                                    ;interrupt routine and flushed
                                    ;by system timer service routine
RxReadPtr  dw 0                     ;read pointer
RxWritePtr dw 0                     ;write pointer

;**** Make variables Public

PUBLIC RxBufferLen,RxBuffer,RxReadPtr,RxWritePtr
PUBLIC OldTimerISR

.Code

Extrn Timer_ISR : PROC


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
;    pushf
;    push ax
;    push dx
    cli

    push  es				  ; Get old vector
    mov   ax,3508h
    int   21h
    mov   dx,es
    mov   ax,bx
    pop   es

    mov   Old_Timer_seg,dx		  ; Store old vector
    mov   Old_Timer_off,ax

    push  ds
    mov   ax, SEG Timer_ISR		  ; Set new vector
    mov   ds,ax
    mov   dx, OFFSET Timer_ISR
    mov   ax,2508h
    int   21h
    pop   ds

    ;--------------------
    ; Return from routine
    ;--------------------
;    pop dx
;    pop ax
;    popf
    sti
    ret

Initialize_Timer ENDP

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
;    pushf
;    push ax
;    push dx
    cli

    push  ds			    ; Restore vector 8h (timer handling)
    mov   dx, Old_Timer_off
    mov   ax, Old_Timer_seg
    mov   ds, ax
    mov   ax, 2508h
    int   21h
    pop   ds

    ;---------------------
    ; Return from routine
    ;---------------------
;    pop dx
;    pop ax
;    popf
    sti
    ret

Restore_Timer ENDP
END

