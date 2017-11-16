;******************************************************************
;
; FILE: ISR.INC
; DATE: 940122
; DESC: Macro file for Hva Packet Monitor
;
;******************************************************************

;-------------------------------
; Primary parameters
;-------------------------------
com_irq         db 3    ;COM port IRQ
com_base        dw 2f8h ;COM port base

;--------------------------------
; Make primary parameters public
;--------------------------------
PUBLIC com_irq, com_base

;------------------------------------------------
;secondary parameters computed from primary ones
;------------------------------------------------
irq_mask        db 0    ;IRQ mask for 8259
PUBLIC irq_mask

even
save_IER        db 0
save_LCR        db 0
save_MCR        db 0
save_DLL        db 0
save_DLM        db 0
save_irq_en     db 0FFh ;save irq enabled in 8259

Old_COM_ISR EQU THIS DWORD
Old_COM_ISR_off  DW 0   ; offset address
Old_COM_ISR_seg  DW 0   ; segment address

;PUBLIC Old_COM_ISR, Old_COM_ISR_seg, Old_COM_ISR_off

OldTimerISR EQU THIS DWORD
Old_Timer_off    DW 0 ; offset address
Old_Timer_seg    DW 0 ; segment address

PUBLIC OldTimerISR, Old_Timer_seg, Old_Timer_off

RXBUFFERLEN EQU 1023                ;in words, must be 2^n-1
RxBuffer   dw RXBUFFERLEN+1 dup(0)  ;Rx buffer storing periods between
PUBLIC RxBuffer                     ;CTS transitions
                                    ;this buffer is filled by CTS transitions
                                    ;interrupt routine and flushed
                                    ;by system timer service routine
RxReadPtr  dw 0                     ;read pointer
RxWritePtr dw 0                     ;write pointer

write_ptr    dw 0
PUBLIC write_ptr
bitcntr      db 8
shiftreg     db 0

even
prev_timer_count dw 0          ; Previous timer count
PUBLIC prev_timer_count
TimerISRActive   db 0          ; Flag to see if we are already in TimerISR

MAXFRAMELEN equ 400h                    ;maximum frame length in bytes
even
RxFrameData     db  MAXFRAMELEN dup(0)  ;frame buffer
RxFrameLen      dw  0                   ;actual frame length (counts bytes)

PUBLIC RxFrameLen

OCW2		   EQU 20h	        ; 8259 port
OCW1		   EQU 21h	        ; 8259 port
EOI		       EQU 20h	        ; End Of Interrupt
IRQ3_EOI	   EQU 01100011b     ; Specified IRQ3 EOI
IRQ4_EOI	   EQU 01100100b     ; Specified IRQ4 EOI
IRQ3_MASK	   EQU 00001000b    ; Specified mask 3
IRQ4_MASK      EQU 00010000b    ; Specified mask 4

;----------------------------------------------------\
; Offset EQUations to reach the regiters of the UART |
;----------------------------------------------------/
RBR	EQU 0		; Receiver Buffer Register (Read Only)
THR	EQU 0		; Transmitter Holding Register (Write Only)
IER	EQU 1		; Interrupt Enable Register
IIR	EQU 2		; Interrupt Ident. Register (Read Only)
LCR	EQU 3		; Line Control Register
MCR	EQU 4		; Modem Control Register
LSR	EQU 5		; Line Status Register
MSR	EQU 6		; Modem Status Register
SCR	EQU 7		; Scratch Register
DLL	EQU 0		; Divisor Latch (LS)
DLM	EQU 1		; Latch (MS)

;----------------------------------------\
; 'Interrupt Enable Register' bit layout |
;----------------------------------------/
ERBFI	    EQU 0001b	; Enable Received Data Available Interrupt
ETBEI	    EQU 0010b	; Enable Transmitter Holding Register Empty Interrupt
ELSI	    EQU 0100b	; Enable Receiver Line Status Interrupt
EDSSI	    EQU 1000b	; Enable MODEM Status Interrupt

;--------------------------------------\
; 'MODEM Control Register' bits layout |
;--------------------------------------/
DTR	        EQU   00000001b
RTS	        EQU   00000010b
OUT1	    EQU   00000100b
OUT2	    EQU   00001000b
UARTLOOP    EQU   00010000b

;--------------------------------------\
; 'MODEM Status Register' bits layout  |
;--------------------------------------/
DCTS	    EQU   00000001b   ; Delta Clear To Send
DDSR	    EQU   00000010b   ; Delta Data Set Ready
TERI	    EQU   00000100b   ; Trailing Edge Ring Indicator
DDCD	    EQU   00001000b   ; Delta Data Carrier Detect
CTS	        EQU   00010000b   ; Clear To Send
DSR	        EQU   00100000b   ; Data Set Ready
RI	        EQU   01000000b   ; Ring Indicator
DCD	        EQU   10000000b   ; Data Carrier Detect


PUSH_ALL    MACRO
	      push  ds
	      push  es
	      push  di
	      push  si
	      push  ax
	      push  bx
	      push  cx
	      push  dx
	    ENDM

POP_ALL     MACRO
	      pop   dx
	      pop   cx
	      pop   bx
	      pop   ax
	      pop   si
	      pop   di
	      pop   es
	      pop   ds
	    ENDM



