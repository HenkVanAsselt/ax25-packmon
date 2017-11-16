
PAGE  59,132

;€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€
;€€                                                                      €€
;€€                             PRMON                                    €€
;€€                                                                      €€
;€€      Created:   16-Feb-93                                            €€
;€€      Version:                                                        €€
;€€      Passes:    5          Analysis Options on: X                    €€
;€€                                                                      €€
;€€                                                                      €€
;€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€

movSeg           macro reg16, unused, Imm16     ; Fixup for Assembler
                 ifidn  <reg16>, <bx>
                 db     0BBh
                 endif
                 ifidn  <reg16>, <cx>
                 db     0B9h
                 endif
                 ifidn  <reg16>, <dx>
                 db     0BAh
                 endif
                 ifidn  <reg16>, <si>
                 db     0BEh
                 endif
                 ifidn  <reg16>, <di>
                 db     0BFh
                 endif
                 ifidn  <reg16>, <bp>
                 db     0BDh
                 endif
                 ifidn  <reg16>, <sp>
                 db     0BCh
                 endif
                 ifidn  <reg16>, <BX>
                 db     0BBH
                 endif
                 ifidn  <reg16>, <CX>
                 db     0B9H
                 endif
                 ifidn  <reg16>, <DX>
                 db     0BAH
                 endif
                 ifidn  <reg16>, <SI>
                 db     0BEH
                 endif
                 ifidn  <reg16>, <DI>
                 db     0BFH
                 endif
                 ifidn  <reg16>, <BP>
                 db     0BDH
                 endif
                 ifidn  <reg16>, <SP>
                 db     0BCH
                 endif
                 dw     seg Imm16
endm
timer_low       equ     6Ch                                     ; (0040:006C=0E0F6h)
data_1e         equ     2Ch                                     ; (79E2:002C=0)
psp_cmd_tail    equ     81h                                     ; (79E2:0081=0)
Old_COMM_Ptr    equ     1972h                                   ; (79E2:1972=0)
Old_int_16h     equ     1976h                                   ; (79E2:1976=0)
Old_int_1Ch     equ     197Ah                                   ; (79E2:197A=0)
Old_int_28h     equ     197Eh                                   ; (79E2:197E=0)
data_34e        equ     1982h                                   ; (79E2:1982=0)
data_35e        equ     1984h                                   ; (79E2:1984=1176h)
data_36e        equ     1986h                                   ; (79E2:1986=0)
Ptr1            equ     198Ah                                   ; (79E2:198A=0)
Ptr3            equ     198Ch                                   ; (79E2:198C=0)
Ptr2            equ     198Eh                                   ; (79E2:198E=0)
Old_timerval    equ     1990h                                   ; (79E2:1990=0)
data_38e        equ     1992h                                   ; (79E2:1992=0)
data_40e        equ     1994h                                   ; (79E2:1994=0)
data_41e        equ     1995h                                   ; (79E2:1995=0)
data_42e        equ     19C4h                                   ; (79E2:19C4=0)
data_43e        equ     1A00h                                   ; (79E2:1A00=0)
data_44e        equ     1A07h                                   ; (79E2:1A07=0)

seg_a           segment byte public
                assume  cs:seg_a, ds:seg_a


                org     100h

prmon           proc    far

start:
                jmp     loc_70                                  ; (0A1B)
IRQ_nr          db      4
COMM_Base       dw      3F8h
Bit_len         dw      7C4h
data_2          dw      0
data_3          dw      0, 79E2h
Timer_cntdwn    dw      24h
bitcntr         db      8
shiftreg        db      0
data_5          db      0
data_6          db      0Ah
                db       63h, 72h, 68h
data_7          db      0

prmon           endp

;€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€
;
;                       External Entry Point
;
;€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€

int_16h_entry   proc    far
                sti                                             ; Enable interrupts
                call    sub_1                                   ; (0159)
                cmp     ah,12h
                ja      loc_1                                   ; Jump if above
                test    ah,0EFh
                jnz     loc_1                                   ; Jump if not zero
                push    ax
                inc     ah
                pushf                                           ; Push flags
                cli                                             ; Disable interrupts
                call    dword ptr cs:Old_int_16h                ; (79E2:1976=0)
                pop     ax
                jz      int_16h_entry                           ; Jump if zero
loc_1:
                cli                                             ; Disable interrupts
                jmp     dword ptr cs:Old_int_16h                ; (79E2:1976=0)
int_16h_entry   endp


;€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€
;
;                       External Entry Point
;
;€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€

int_1Ch_entry   proc    far
                sti                                             ; Enable interrupts
                cmp     cs:Timer_cntdwn,0                       ; (79E2:010E=24h)
                je      loc_2                                   ; Jump if equal
                dec     cs:Timer_cntdwn                         ; (79E2:010E=24h)
loc_2:
                jmp     dword ptr cs:Old_int_1Ch                ; (79E2:197A=0)
int_1Ch_entry   endp


;€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€
;
;                       External Entry Point
;
;€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€

int_24h_entry   proc    far
                mov     al,3
                iret                                            ; Interrupt return
int_24h_entry   endp


;€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€
;
;                       External Entry Point
;
;€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€

int_28h_entry   proc    far
                sti                                             ; Enable interrupts
                call    sub_2                                   ; (0168)
                cli                                             ; Disable interrupts
                jmp     dword ptr cs:Old_int_28h                ; (79E2:197E=0)
int_28h_entry   endp


;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_1           proc    near
                push    bx
                push    ds
                lds     bx,dword ptr cs:data_36e                ; (79E2:1986=0) Load 32 bit ptr
                cmp     byte ptr [bx],0
                pop     ds
                pop     bx
                jz      loc_3                                   ; Jump if zero
                retn
sub_1           endp


;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_2           proc    near
loc_3:
                push    ax
                push    ds
                mov     ax,cs
                mov     ds,ax
                cmp     data_5,0                                ; (79E2:0112=0)
                jne     loc_7                                   ; Jump if not equal
                cmp     Timer_cntdwn,0                          ; (79E2:010E=24h)
                jne     loc_7                                   ; Jump if not equal
                mov     data_5,0FFh                             ; (79E2:0112=0)
                mov     ds:data_35e,ss                          ; (79E2:1984=1176h)
                mov     ds:data_34e,sp                          ; (79E2:1982=0)
                cli                                             ; Disable interrupts
                mov     ss,ax
                mov     sp,1B08h
                sti                                             ; Enable interrupts
                push    bx
                push    cx
                push    dx
                push    si
                push    di
                push    es
                mov     es,ax
                call    sub_3                                   ; (01F3)
                mov     dx,COMM_Base                            ; (79E2:0104=3F8h)
                add     dl,6
                in      al,dx                                   ; port 3FEh, RS232-1 modem stat
                mov     al,3Ah                                  ; ':'
                call    sub_13                                  ; (0539)
                mov     ah,62h                                  ; 'b'
                int     21h                                     ; DOS Services  ah=function 62h
                                                                ;  get progrm seg prefix addr bx
                mov     data_2,bx                               ; (79E2:0108=0)
                mov     bx,cs
                mov     ah,50h                                  ; 'P'
                int     21h                                     ; DOS Services  ah=function 50h
                                                                ;  set active PSP segmnt from bx
                cld                                             ; Clear direction
loc_4:
                call    sub_4                                   ; (02E2)
                jnc     loc_5                                   ; Jump if carry=0
                cmp     bl,3
                je      loc_6                                   ; Jump if equal
                jmp     short loc_4                             ; (01B7)
loc_5:
                call    sub_6                                   ; (0400)
                call    sub_8                                   ; (042C)
                call    sub_19                                  ; (05F5)
                mov     al,2Eh                                  ; '.'
                call    sub_13                                  ; (0539)
                jmp     short loc_4                             ; (01B7)
loc_6:
                mov     bx,data_2                               ; (79E2:0108=0)
                mov     ah,50h                                  ; 'P'
                int     21h                                     ; DOS Services  ah=function 50h
                                                                ;  set active PSP segmnt from bx
                pop     es
                pop     di
                pop     si
                pop     dx
                pop     cx
                pop     bx
                cli                                             ; Disable interrupts
                mov     ss,ds:data_35e                          ; (79E2:1984=1176h)
                mov     sp,ds:data_34e                          ; (79E2:1982=0)
                sti                                             ; Enable interrupts
                mov     data_5,0                                ; (79E2:0112=0)
loc_7:
                pop     ds
                pop     ax
                retn
sub_2           endp


;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_3           proc    near
                mov     cl,7
                mov     bx,1000h
                mov     ax,Bit_len                              ; (79E2:0106=7C4h)
                mul     bx                                      ; dx:ax = reg * ax
                mov     bx,4F85h
                div     bx                                      ; ax,dx rem=dx:ax/reg
                mov     Timer_cntdwn,ax                         ; (79E2:010E=24h)
                retn
sub_3           endp


;€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€
;
;                       External Entry Point
;
;€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€

int_0Ch_entry   proc    far
                push    ax
                push    bx
                push    cx
                push    dx
                push    ds
                mov     ax,cs
                mov     ds,ax
                mov     dx,COMM_Base                            ; (79E2:0104=3F8h)
                add     dl,6
                in      al,dx                                   ; port 3FEh, RS232-1 modem stat
                and     al,1
                jz      jmp_do_EOI                              ; Jump if zero
                mov     al,0
                out     43h,al                                  ; port 43h, 8253 wrt timr mode
                                                                ;  al = 0, timer 0 count mode
                in      al,40h                                  ; port 40h, 8253 timer 0 clock
                mov     ah,al
                in      al,40h                                  ; port 40h, 8253 timer 0 clock
                xchg    al,ah
                mov     dx,ds:Old_timerval                      ; (79E2:1990=0)
                sub     dx,ax
                mov     ds:Old_timerval,ax                      ; (79E2:1990=0)
                mov     ax,Bit_len                              ; (79E2:0106=7C4h)
                mov     bx,ax
                shr     ax,1                                    ; Shift w/zeros fill
                cmp     dx,ax
                jb      jmp_do_EOI                              ; Jump if below
                add     ax,bx
                xor     cx,cx                                   ; Zero register
loc_8:
                cmp     dx,ax
                jb      loc_9                                   ; Jump if below
                add     ax,bx
                inc     cx
                cmp     cl,7
                jbe     loc_8                                   ; Jump if below or =
loc_9:
                cmp     cl,6
                jb      get_tmp_data                            ; Jump if below
                mov     bx,ds:Ptr2                              ; (79E2:198E=0)
                ja      store_00_octet                          ; Jump if above
                cmp     bitcntr,7                               ; (79E2:0110=8)
                je      loc_10                                  ; Jump if equal
store_00_octet:
                mov     al,0
                call    StoreOctet                              ; (02D4)
                call    StoreOctet                              ; (02D4)
                mov     dl,8
                jmp     short save_tmp_data                     ; (02B4)
loc_10:
                mov     cx,ds:Ptr1                              ; (79E2:198A=0)
                sub     cx,bx
                jnc     save_framelen                           ; Jump if carry=0
                add     cx,1000h
save_framelen:
                sub     cx,2
                jbe     store_00_octet                          ; Jump if below or =
                cmp     cx,14Ah
                ja      store_00_octet                          ; Jump if above
                mov     al,cl
                call    StoreOctet                              ; (02D4)
                mov     al,ch
                call    StoreOctet                              ; (02D4)
                mov     bx,ds:Ptr1                              ; (79E2:198A=0)
                mov     ds:Ptr2,bx                              ; (79E2:198E=0)
                jmp     short store_00_octet                    ; (025D)
jmp_do_EOI:
                jmp     short do_EOI                            ; (02BF)
get_tmp_data:
                mov     dl,bitcntr                              ; (79E2:0110=8)
                mov     bx,ds:Ptr1                              ; (79E2:198A=0)
                mov     al,shiftreg                             ; (79E2:0111=0)
                jcxz    store_0_bit                             ; Jump if cx=0
                mov     ah,cl
store_1_bits:
                stc                                             ; Set carry flag
                call    StoreBit                                ; (02CC)
                loop    store_1_bits                            ; Loop if cx > 0

                cmp     ah,5
                je      save_tmp_data                           ; Jump if equal
store_0_bit:
                clc                                             ; Clear carry flag
                call    StoreBit                                ; (02CC)
save_tmp_data:
                mov     shiftreg,al                             ; (79E2:0111=0)
                mov     bitcntr,dl                              ; (79E2:0110=8)
                mov     ds:Ptr1,bx                              ; (79E2:198A=0)
do_EOI:
                mov     al,IRQ_nr                               ; (79E2:0103=4)
                add     al,60h                                  ; '`'
                pop     ds
                pop     dx
                pop     cx
                pop     bx
                out     20h,al                                  ; port 20h, 8259-1 int command
                pop     ax
                iret                                            ; Interrupt return
int_0Ch_entry   endp


;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

StoreBit        proc    near
                rcr     al,1                                    ; Rotate thru carry
                dec     dl
                jnz     ret_Sub4_Sub5                           ; Jump if not zero
                mov     dl,8

;ﬂﬂﬂﬂ External Entry into Subroutine ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ

StoreOctet:
                mov     byte ptr opt11+0Ch[bx],al               ; (79E2:0972=75h)
                inc     bx
                cmp     bx,1000h
                jb      ret_Sub4_Sub5                           ; Jump if below
                xor     bx,bx                                   ; Zero register
ret_Sub4_Sub5:
                retn
StoreBit        endp


;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_4           proc    near
                push    bp
                mov     si,ds:Ptr3                              ; (79E2:198C=0)
                mov     di,772h
                mov     bl,3
                mov     dx,si
                cli                                             ; Disable interrupts
                call    sub_11                                  ; (050D)
                mov     cl,al
                call    sub_11                                  ; (050D)
                sti                                             ; Enable interrupts
                mov     ch,al
                jcxz    loc_11                                  ; Jump if cx=0
                cmp     cx,14Ah
                jbe     loc_13                                  ; Jump if below or =
                mov     al,0F0h
                call    sub_13                                  ; (0539)
                mov     dx,ds:Ptr2                              ; (79E2:198E=0)
loc_11:
                mov     si,dx
loc_12:
                jmp     loc_28                                  ; (03DE)
loc_13:
                dec     bl
                mov     dx,0FFFFh
                xor     bh,bh                                   ; Zero register
                mov     byte ptr ds:data_38e+1,bh               ; (79E2:1993=0)
loc_14:
                inc     bh
                cmp     bh,0Ah
                ja      loc_23                                  ; Jump if above
                mov     ah,6
loc_15:
                jcxz    loc_12                                  ; Jump if cx=0
                dec     cx
                call    sub_12                                  ; (051B)
                shr     al,1                                    ; Shift w/zeros fill
                jc      loc_16                                  ; Jump if carry Set
                cmp     al,20h                                  ; ' '
                jbe     loc_17                                  ; Jump if below or =
loc_16:
                stosb                                           ; Store al to es:[di]
                jc      loc_22                                  ; Jump if carry Set
loc_17:
                dec     ah
                jnz     loc_15                                  ; Jump if not zero
                jcxz    loc_12                                  ; Jump if cx=0
                dec     cx
                call    sub_12                                  ; (051B)
                test    al,80h
                jz      loc_19                                  ; Jump if zero
                cmp     bh,2
                ja      loc_18                                  ; Jump if above
                or      byte ptr ds:data_38e+1,bh               ; (79E2:1993=0)
                jmp     short loc_19                            ; (0353)
loc_18:
                or      byte ptr ds:data_38e+1,4                ; (79E2:1993=0)
loc_19:
                shr     al,1                                    ; Shift w/zeros fill
                pushf                                           ; Push flags
                and     al,0Fh
                jz      loc_21                                  ; Jump if zero
                mov     byte ptr [di],2Dh                       ; '-'
                inc     di
                add     al,30h                                  ; '0'
                cmp     al,39h                                  ; '9'
                jbe     loc_20                                  ; Jump if below or =
                mov     byte ptr [di],31h                       ; '1'
                inc     di
                sub     al,0Ah
loc_20:
                stosb                                           ; Store al to es:[di]
loc_21:
                popf                                            ; Pop flags
                jc      loc_22                                  ; Jump if carry Set
                mov     al,2Ch                                  ; ','
                stosb                                           ; Store al to es:[di]
                jmp     short loc_14                            ; (031B)
loc_22:
                mov     al,0F9h
                stosb                                           ; Store al to es:[di]
                cmp     cx,3
                jb      close_file                              ; Jump if below
                call    sub_12                                  ; (051B)
                dec     cx
                mov     ds:data_38e,al                          ; (79E2:1992=0)
                cmp     cx,103h
loc_23:
                ja      close_file                              ; Jump if above
                xor     bh,bh                                   ; Zero register
                sub     cx,2
                jz      loc_24                                  ; Jump if zero
                dec     cx
                call    sub_12                                  ; (051B)
                mov     bh,al
loc_24:
                jcxz    loc_27                                  ; Jump if cx=0
                mov     bp,di
                test    data_7,40h                              ; (79E2:0117=0) '@'
                jz      locloop_25                              ; Jump if zero
                mov     ax,0A0Dh
                stosw                                           ; Store ax to es:[di]

locloop_25:
                call    sub_12                                  ; (051B)
                cmp     al,1Ah
                je      loc_26                                  ; Jump if equal
                cmp     al,0Ah
                je      loc_26                                  ; Jump if equal
                stosb                                           ; Store al to es:[di]
                cmp     al,0Dh
                jne     loc_26                                  ; Jump if not equal
                mov     al,0Ah
                stosb                                           ; Store al to es:[di]
loc_26:
                loop    locloop_25                              ; Loop if cx > 0

                mov     cx,di
                sub     cx,bp
loc_27:
                dec     bl
                call    sub_12                                  ; (051B)
                call    sub_12                                  ; (051B)
                cmp     dx,0F0B8h
                jne     loc_28                                  ; Jump if not equal
                mov     ax,ds:data_38e                          ; (79E2:1992=0)
                dec     bl
                jmp     short loc_29                            ; (03DF)
close_file:
                add     si,cx
                cmp     si,1000h
                jbe     loc_28                                  ; Jump if below or =
                sub     si,1000h
loc_28:
                stc                                             ; Set carry flag
loc_29:
                mov     ds:Ptr3,si                              ; (79E2:198C=0)
                mov     si,bp
                pop     bp
                retn
sub_4           endp


;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_5           proc    near
                push    si
                push    di
                mov     si,data_43e                             ; (79E2:1A00=0)
                mov     di,offset data_10                       ; (79E2:0772=42h)
                cmp     byte ptr [si],0
                je      loc_31                                  ; Jump if equal
loc_30:
                lodsb                                           ; String [si] to al
                or      al,al                                   ; Zero ?
                jz      loc_31                                  ; Jump if zero
                scasb                                           ; Scan es:[di] for al
                jz      loc_30                                  ; Jump if zero
                stc                                             ; Set carry flag
loc_31:
                pop     di
                pop     si
                retn
sub_5           endp


;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_6           proc    near
                push    ax
                push    cx
                push    dx
                push    di
                mov     ah,2Ch                                  ; ','
                int     21h                                     ; DOS Services  ah=function 2Ch
                                                                ;  get time, cx=hrs/min, dh=sec
                mov     di,offset data_9                        ; (79E2:0769=68h)
                mov     al,ch
                call    sub_7                                   ; (0421)
                inc     di
                mov     al,cl
                call    sub_7                                   ; (0421)
                inc     di
                mov     al,dh
                call    sub_7                                   ; (0421)
                pop     di
                pop     dx
                pop     cx
                pop     ax
                retn
sub_6           endp


;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_7           proc    near
                xor     ah,ah                                   ; Zero register
                div     data_6                                  ; (79E2:0113=0Ah) al,ah rem = ax/data
                add     ax,3030h
                stosw                                           ; Store ax to es:[di]
                retn
sub_7           endp


;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_8           proc    near
                push    ax
                push    bx
                test    data_7,10h                              ; (79E2:0117=0)
                jnz     loc_34                                  ; Jump if not zero
                jcxz    loc_34                                  ; Jump if cx=0
                xor     bx,bx                                   ; Zero register
                push    cx
                push    si

locloop_32:
                lodsb                                           ; String [si] to al
                cmp     al,7Fh
                jb      loc_33                                  ; Jump if below
                inc     bx
loc_33:
                loop    locloop_32                              ; Loop if cx > 0

                pop     si
                pop     cx
                shl     bx,1                                    ; Shift w/zeros fill
                shl     bx,1                                    ; Shift w/zeros fill
                cmp     bx,cx
                jb      loc_34                                  ; Jump if below
                mov     word ptr [si],2323h
                mov     di,si
                mov     cx,2
                add     di,cx
loc_34:
                pop     bx
                pop     ax
                retn
sub_8           endp


;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_9           proc    near
                push    cx
                push    si
                mov     cx,3
                mov     si,114h

locloop_35:
                shr     ah,1                                    ; Shift w/zeros fill
                jnc     loc_36                                  ; Jump if carry=0
                mov     al,2Ch                                  ; ','
                call    sub_14                                  ; (054D)
                mov     al,[si]
                call    sub_14                                  ; (054D)
loc_36:
                inc     si
                loop    locloop_35                              ; Loop if cx > 0

                pop     si
                pop     cx
                retn
sub_9           endp


;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_10          proc    near
                push    ax
                push    cx
                mov     ah,dl
                test    ah,1
                jz      loc_39                                  ; Jump if zero
                and     ah,0EFh
                cmp     ah,3
                mov     cx,5549h
                jz      loc_38                                  ; Jump if zero
                cmp     ah,83h
                mov     cx,4652h
                jz      loc_38                                  ; Jump if zero
                cmp     ah,63h                                  ; 'c'
                mov     cx,5541h
                jz      loc_38                                  ; Jump if zero
                cmp     ah,43h                                  ; 'C'
                mov     cx,4449h
                jz      loc_38                                  ; Jump if zero
                cmp     ah,2Fh                                  ; '/'
                mov     cx,5341h
                jz      loc_38                                  ; Jump if zero
                cmp     ah,0Fh
                mov     cx,444Dh
                jz      loc_38                                  ; Jump if zero
                and     ah,0Fh
                cmp     ah,1
                mov     cx,5252h
                jz      loc_37                                  ; Jump if zero
                cmp     ah,5
                mov     cx,524Eh
                jz      loc_37                                  ; Jump if zero
                cmp     ah,9
                mov     cx,524Ah
                jz      loc_37                                  ; Jump if zero
                mov     cx,3F3Fh
loc_37:
                mov     al,ch
                call    sub_14                                  ; (054D)
                mov     al,cl
                jmp     short loc_40                            ; (04F0)
loc_38:
                mov     al,ch
                call    sub_14                                  ; (054D)
                mov     al,cl
                jmp     short loc_41                            ; (04FD)
loc_39:
                mov     al,49h                                  ; 'I'
                call    sub_14                                  ; (054D)
                mov     al,dl
                shr     al,1                                    ; Shift w/zeros fill
                and     al,7
                add     al,30h                                  ; '0'
loc_40:
                call    sub_14                                  ; (054D)
                mov     al,dl
                mov     cl,5
                shr     al,cl                                   ; Shift w/zeros fill
                and     al,7
                add     al,30h                                  ; '0'
loc_41:
                call    sub_14                                  ; (054D)
                test    dl,10h
                jz      loc_42                                  ; Jump if zero
                mov     al,2Bh                                  ; '+'
                call    sub_14                                  ; (054D)
loc_42:
                pop     cx
                pop     ax
                retn
sub_10          endp


;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_11          proc    near
                mov     al,byte ptr opt11+0Ch[si]               ; (79E2:0972=75h)
                inc     si
                cmp     si,1000h
                jb      loc_ret_43                              ; Jump if below
                xor     si,si                                   ; Zero register

loc_ret_43:
                retn
sub_11          endp


;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_12          proc    near
                call    sub_11                                  ; (050D)
                push    bx
                push    cx
                xor     dl,al
                mov     bl,dl
                mov     cl,4
                shl     bl,cl                                   ; Shift w/zeros fill
                xor     bl,dl
                xor     bh,bh                                   ; Zero register
                shl     bx,cl                                   ; Shift w/zeros fill
                xor     dx,bx
                shr     bx,1                                    ; Shift w/zeros fill
                xchg    dl,dh
                xor     dx,bx
                pop     cx
                pop     bx
                retn
sub_12          endp


;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_13          proc    near
                test    data_7,80h                              ; (79E2:0117=0)
                jz      loc_ret_44                              ; Jump if zero
                call    sub_14                                  ; (054D)
                cmp     al,41h                                  ; 'A'
                jb      loc_ret_44                              ; Jump if below
                mov     ax,0E07h
                int     10h                                     ; Video display   ah=functn 0Eh
                                                                ;  write char al, teletype mode

loc_ret_44:
                retn
sub_13          endp


;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_14          proc    near
loc_45:
                push    ax
                push    bx
                cmp     al,7
                je      loc_49                                  ; Jump if equal
                mov     bl,7
                mov     ah,0Eh
                int     10h                                     ; Video display   ah=functn 0Eh
                                                                ;  write char al, teletype mode
                cmp     al,0Dh
                jne     loc_49                                  ; Jump if not equal
                test    data_7,2                                ; (79E2:0117=0)
                jz      loc_49                                  ; Jump if zero
loc_46:
                push    ds
                mov     ax,40h
                mov     ds,ax
                mov     ah,9
loc_47:
                mov     al,ds:timer_low                         ; (0040:006C=0Dh)
loc_48:
                cmp     al,ds:timer_low                         ; (0040:006C=0Dh)
                je      loc_48                                  ; Jump if equal
                dec     ah
                jnz     loc_47                                  ; Jump if not zero
                pop     ds
loc_49:
                pop     bx
                pop     ax
                retn
sub_14          endp


;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_15          proc    near
                push    ax
                shr     al,1                                    ; Shift w/zeros fill
                shr     al,1                                    ; Shift w/zeros fill
                shr     al,1                                    ; Shift w/zeros fill
                shr     al,1                                    ; Shift w/zeros fill
                call    sub_16                                  ; (058C)
                pop     ax
                and     al,0Fh

;ﬂﬂﬂﬂ External Entry into Subroutine ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ

sub_16:
                add     al,30h                                  ; '0'
                cmp     al,3Ah                                  ; ':'
                jb      loc_50                                  ; Jump if below
                add     al,7
loc_50:
                jmp     short loc_45                            ; (054D)
sub_15          endp


;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_17          proc    near
loc_51:
                mov     ah,1
                int     16h                                     ; Keyboard i/o  ah=function 01h
                                                                ;  get status, if zf=0  al=char
                jz      loc_52                                  ; Jump if zero
                mov     ah,0
                int     16h                                     ; Keyboard i/o  ah=function 00h
                                                                ;  get keybd char in al, ah=scan
                cmp     al,1Bh
                je      loc_ret_56                              ; Jump if equal
loc_52:
                call    sub_4                                   ; (02E2)
                jc      loc_51                                  ; Jump if carry Set
                or      cx,cx                                   ; Zero ?
                jnz     loc_53                                  ; Jump if not zero
                test    data_7,1                                ; (79E2:0117=0)
                jnz     loc_51                                  ; Jump if not zero
loc_53:
                call    sub_8                                   ; (042C)
                call    sub_6                                   ; (0400)
                call    sub_19                                  ; (05F5)
                mov     dl,al
                mov     si,offset data_8                        ; (79E2:0766=0Dh)
loc_54:
                lodsb                                           ; String [si] to al
                call    sub_14                                  ; (054D)
                cmp     al,0F9h
                jne     loc_54                                  ; Jump if not equal
                call    sub_10                                  ; (0477)
                call    sub_9                                   ; (045B)
                jcxz    loc_51                                  ; Jump if cx=0
                mov     al,2Ch                                  ; ','
                call    sub_14                                  ; (054D)
                mov     al,bh
                call    sub_15                                  ; (057D)
                mov     al,3Ah                                  ; ':'
                call    sub_14                                  ; (054D)

locloop_55:
                lodsb                                           ; String [si] to al
                call    sub_14                                  ; (054D)
                loop    locloop_55                              ; Loop if cx > 0

                jmp     short loc_51                            ; (0596)

loc_ret_56:
                retn
sub_17          endp


;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_18          proc    near
                cmp     al,61h                                  ; 'a'
                jb      loc_ret_57                              ; Jump if below
                cmp     al,7Ah                                  ; 'z'
                ja      loc_ret_57                              ; Jump if above
                sub     al,20h                                  ; ' '

loc_ret_57:
                retn
sub_18          endp


;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_19          proc    near
                push    ax
                push    bx
                push    cx
                push    dx
                test    data_7,4                                ; (79E2:0117=0)
                jz      loc_59                                  ; Jump if zero
                jcxz    loc_59                                  ; Jump if cx=0
                call    sub_5                                   ; (03E7)
                jc      loc_59                                  ; Jump if carry Set
                push    di
                push    si
                mov     di,data_42e                             ; (79E2:19C4=0)
                mov     dx,di
                mov     si,data_40e                             ; (79E2:1994=0)
                call    sub_20                                  ; (06A5)
                mov     si,offset data_10                       ; (79E2:0772=42h)
loc_58:
                lodsb                                           ; String [si] to al
                cmp     al,2Dh                                  ; '-'
                je      loc_58                                  ; Jump if equal
                cmp     al,30h                                  ; '0'
                jb      loc_60                                  ; Jump if below
                stosb                                           ; Store al to es:[di]
                inc     cx
                jmp     short loc_58                            ; (0617)
loc_59:
                jmp     short loc_66                            ; (06A0)
loc_60:
                mov     ax,542Eh
                stosw                                           ; Store ax to es:[di]
                mov     ax,5458h
                stosw                                           ; Store ax to es:[di]
                mov     al,0
                stosb                                           ; Store al to es:[di]
                pop     si
                pop     di
                push    es
                mov     ax,3524h
                int     21h                                     ; DOS Services  ah=function 35h
                                                                ;  get intrpt vector al in es:bx
                mov     data_3,bx                               ; (79E2:010A=0)
                mov     word ptr data_3+2,es                    ; (79E2:010C=79E2h)
                pop     es
                push    dx
                mov     dx,offset int_24h_entry
                mov     ax,2524h
                int     21h                                     ; DOS Services  ah=function 25h
                                                                ;  set intrpt vector al to ds:dx
                pop     dx
                mov     ax,3D01h
                int     21h                                     ; DOS Services  ah=function 3Dh
                                                                ;  open file, al=mode,name@ds:dx
                jnc     loc_61                                  ; Jump if carry=0
                xor     cx,cx                                   ; Zero register
                mov     ah,3Ch                                  ; '<'
                int     21h                                     ; DOS Services  ah=function 3Ch
                                                                ;  create/truncate file @ ds:dx
                jnc     loc_61                                  ; Jump if carry=0
                mov     al,13h
                call    sub_13                                  ; (0539)
                jmp     short loc_65                            ; (0695)
loc_61:
                mov     bx,ax
                mov     ax,4202h
                xor     cx,cx                                   ; Zero register
                xor     dx,dx                                   ; Zero register
                int     21h                                     ; DOS Services  ah=function 42h
                                                                ;  move file ptr, cx,dx=offset
                mov     dx,offset data_8                        ; (79E2:0766=0Dh)
                test    data_7,20h                              ; (79E2:0117=0) ' '
                jz      loc_62                                  ; Jump if zero
                mov     dx,si
loc_62:
                mov     cx,di
                sub     cx,dx
                mov     ah,40h                                  ; '@'
                int     21h                                     ; DOS Services  ah=function 40h
                                                                ;  write file cx=bytes, to ds:dx
                jc      loc_63                                  ; Jump if carry Set
                cmp     ax,cx
                je      loc_64                                  ; Jump if equal
loc_63:
                mov     ax,0E07h
                int     10h                                     ; Video display   ah=functn 0Eh
                                                                ;  write char al, teletype mode
                mov     al,57h                                  ; 'W'
                call    sub_13                                  ; (0539)
loc_64:
                mov     ah,3Eh                                  ; '>'
                int     21h                                     ; DOS Services  ah=function 3Eh
                                                                ;  close file, bx=file handle
loc_65:
                push    ds
                lds     dx,dword ptr data_3                     ; (79E2:010A=0) Load 32 bit ptr
                mov     ax,2524h
                int     21h                                     ; DOS Services  ah=function 25h
                                                                ;  set intrpt vector al to ds:dx
                pop     ds
loc_66:
                pop     dx
                pop     cx
                pop     bx
                pop     ax
                retn
sub_19          endp


;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_20          proc    near
loc_67:
                lodsb                                           ; String [si] to al
                stosb                                           ; Store al to es:[di]
                or      al,al                                   ; Zero ?
                jnz     loc_67                                  ; Jump if not zero
                dec     di
                retn
sub_20          endp

loc_68:
                call    sub_17                                  ; (0596)
                call    sub_22                                  ; (0723)
                mov     dx,offset exit_msg                      ; (79E2:0752=0Dh)
                mov     ah,9
                int     21h                                     ; DOS Services  ah=function 09h
                                                                ;  display char string at ds:dx
                mov     ax,4C00h
                int     21h                                     ; DOS Services  ah=function 4Ch
                                                                ;  terminate with al=return code

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_21          proc    near
                mov     dx,2
                cli                                             ; Disable interrupts
                mov     ds:Ptr1,dx                              ; (79E2:198A=0)
                xor     dx,dx                                   ; Zero register
                mov     ds:Ptr2,dx                              ; (79E2:198E=0)
                mov     word ptr opt11+0Ch,dx                   ; (79E2:0972=6E75h)
                mov     bitcntr,8                               ; (79E2:0110=8)
                sti                                             ; Enable interrupts
                mov     ds:Ptr3,dx                              ; (79E2:198C=0)
                mov     al,IRQ_nr                               ; (79E2:0103=4)
                mov     cl,al
                add     al,8
                push    es
                mov     ah,35h                                  ; '5'
                int     21h                                     ; DOS Services  ah=function 35h
                                                                ;  get intrpt vector al in es:bx
                mov     ds:Old_COMM_Ptr,bx                      ; (79E2:1972=0)
                mov     word ptr ds:Old_COMM_Ptr+2,es           ; (79E2:1974=79E2h)
                pop     es
                mov     al,cl
                add     al,8
                mov     ah,25h                                  ; '%'
                mov     dx,offset int_0Ch_entry
                int     21h                                     ; DOS Services  ah=function 25h
                                                                ;  set intrpt vector al to ds:dx
                mov     al,0C1h
                out     20h,al                                  ; port 20h, 8259-1 int command
                in      al,21h                                  ; port 21h, 8259-1 int IMR
                mov     ah,0FEh
                stc                                             ; Set carry flag
                rcl     ah,cl                                   ; Rotate thru carry
                and     al,ah
                out     21h,al                                  ; port 21h, 8259-1 int comands
                mov     dx,COMM_Base                            ; (79E2:0104=3F8h)
                add     dl,6
                in      al,dx                                   ; port 3FEh, RS232-1 modem stat
                sub     dl,2
                mov     al,9
                out     dx,al                                   ; port 3FCh, RS232-1 modem ctrl
                dec     dx
                mov     al,43h                                  ; 'C'
                out     dx,al                                   ; port 3FBh, RS232-1 line cntrl
                sub     dl,2
                mov     al,8
                out     dx,al                                   ; port 3F9h, RS232-1 int enable
                retn
sub_21          endp


;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_22          proc    near
                mov     dx,es:COMM_Base                         ; (79E2:0104=3F8h)
                inc     dx
                mov     al,0
                out     dx,al                                   ; port 3F9h, RS232-1 int enable
                                                                ;  al = 0, all interrupts off
                add     dl,3
                out     dx,al                                   ; port 3FCh, RS232-1 modem ctrl
                dec     dx
                mov     al,3
                mov     cl,es:IRQ_nr                            ; (79E2:0103=4)
                in      al,21h                                  ; port 21h, 8259-1 int IMR
                mov     ah,1
                shl     ah,cl                                   ; Shift w/zeros fill
                or      al,ah
                out     21h,al                                  ; port 21h, 8259-1 int comands
                mov     al,cl
                add     al,8
                mov     ah,25h                                  ; '%'
                push    ds
                lds     dx,dword ptr es:Old_COMM_Ptr            ; (79E2:1972=0) Load 32 bit ptr
                int     21h                                     ; DOS Services  ah=function 25h
                                                                ;  set intrpt vector al to ds:dx
                pop     ds
                retn
sub_22          endp

exit_msg        db      0Dh, 0Ah, 0Ah, 'PRMON beendet.', 0Dh
                db      0Ah, '$'
data_8          db      0Dh
                db       0Ah, 10h
data_9          db      'hh:mm:ss '
data_10         db      42h
                db       65h, 6Eh, 94h
                db      'tigt DOS ab 3.0', 0Dh, 0Ah
                db      7
                db      '$'
                db      'Pfad mu'
                db      0E1h
                db      ' Laufwerk enthalten, z.B. C:\ od'
                db      'er  C:\PR', 0Dh, 0Ah
                db       07h, 24h
copy_right      db      'Packet-Radio-Mon'
                db      'itor 1.08 '
                db      0F9h
                db      ' (c) DC1YB '
                db      0F9h
                db      ' ESC=Ende '
                db      0F9h
                db      ' PRMON /? = Optionenliste', 0Dh, 0Ah
                db      0Ah, '$'
option_lst      db      'ƒƒƒƒƒOptionen vo'
                db       6Eh, 20h, 50h, 52h, 4Dh, 4Fh
                db       4Eh,0C4h,0C4h,0C4h,0C4h,0C4h
                db      0C4h
                db       0Dh, 0Ah
opt1            db      '/2  COM2 statt C'
                db       4Fh, 4Dh, 31h, 20h
                db      'verwenden', 0Dh, 0Ah
opt2            db      '/A  PCs mit halb'
                db      'er Timerfreq.', 0Dh, 0Ah
opt3            db      '/B  Auch  BinÑrdaten anzeigen', 0Dh
                db      0Ah
opt4            db      '/C  CRLF nach He'
                db      'ader einf'
                db       81h, 67h, 65h, 6Eh, 0Dh, 0Ah
opt5            db      '/D  Zeilenweise '
                db       20h, 20h, 56h, 65h, 72h, 7Ah
                db       94h
                db      'gerung', 0Dh, 0Ah
opt6            db      '/H  Header nicht'
                db      ' mitspeichern', 0Dh, 0Ah
opt7            db      '/I  Nur  Info-Bl'
                db       94h, 63h
                db      'ke anzeigen', 0Dh, 0Ah
opt8            db      '/R  Resident  (und Sp'
                db       65h, 69h, 63h, 68h, 65h, 72h
opt9            db      'n)', 0Dh, 0Ah, '/S  Rufzeichenwe'
                db      'ise speicher'
opt10           db      'n', 0Dh, 0Ah, '/Sx Nur Ziel-Call'
                db      ' x speichern', 0Dh, 0Ah, 'Mit '
opt11           db      '/R  sind /I und /S aktiv;'
                db       0Dh, 0Ah
opt12           db      'erneut laden = deinstallieren', 0Dh
                db      0Ah
Copyr2          db      '(C) DC1YB @ Shamrock Software', 0Dh
                db      0Ah, '<Public Domain/Noncommercia'
                db      'l>', 0Dh, 0Ah, '$'
loc_69:
                lds     dx,dword ptr es:Old_int_16h             ; (79E2:1976=0) Load 32 bit ptr
                mov     ax,2516h
                int     21h                                     ; DOS Services  ah=function 25h
                                                                ;  set intrpt vector al to ds:dx
                lds     dx,dword ptr es:Old_int_1Ch             ; (79E2:197A=0) Load 32 bit ptr
                mov     ax,251Ch
                int     21h                                     ; DOS Services  ah=function 25h
                                                                ;  set intrpt vector al to ds:dx
                lds     dx,dword ptr es:Old_int_28h             ; (79E2:197E=0) Load 32 bit ptr
                mov     ax,2528h
                int     21h                                     ; DOS Services  ah=function 25h
                                                                ;  set intrpt vector al to ds:dx
                call    sub_22                                  ; (0723)
                push    cs
                pop     ds
                mov     ah,49h                                  ; 'I'
                int     21h                                     ; DOS Services  ah=function 49h
                                                                ;  release memory block, es=seg
                mov     es,es:data_1e                           ; (79E2:002C=0)
                mov     ah,49h                                  ; 'I'
                int     21h                                     ; DOS Services  ah=function 49h
                                                                ;  release memory block, es=seg
                mov     dx,offset exit_msg                      ; (79E2:0752=0Dh)
                mov     ah,9
                int     21h                                     ; DOS Services  ah=function 09h
                                                                ;  display char string at ds:dx
                mov     ax,4CFFh
                int     21h                                     ; DOS Services  ah=function 4Ch
                                                                ;  terminate with al=return code
loc_70:
                mov     ax,3516h
                int     21h                                     ; DOS Services  ah=function 35h
                                                                ;  get intrpt vector al in es:bx
                cmp     bx,118h
                je      loc_69                                  ; Jump if equal
                push    cs
                pop     es
loc_71:
                mov     ah,30h                                  ; '0'
                int     21h                                     ; DOS Services  ah=function 30h
                                                                ;  get DOS version number ax
                cmp     al,3
                mov     dx,772h
                jc      loc_74                                  ; Jump if carry Set
                cld                                             ; Clear direction
                mov     si,psp_cmd_tail                         ; (79E2:0081=0)
                mov     di,1994h
                mov     byte ptr ds:data_43e,0                  ; (79E2:1A00=0)
loc_72:
                lodsb                                           ; String [si] to al
                cmp     word ptr Copyr2+0Dh,6168h               ; (79E2:09AD=6168h)
                jne     loc_72                                  ; Jump if not equal
                cmp     al,20h                                  ; ' '
                je      loc_72                                  ; Jump if equal
                jc      loc_76                                  ; Jump if carry Set
                cmp     al,2Fh                                  ; '/'
                je      loc_75                                  ; Jump if equal
                call    sub_18                                  ; (05EA)
                stosb                                           ; Store al to es:[di]
                cmp     word ptr Copyr2+6,5931h                 ; (79E2:09A6=5931h)
                je      loc_72                                  ; Jump if equal
loc_73:
                pop     di
                mov     dx,offset option_lst                    ; (79E2:080D=0C4h)
loc_74:
                mov     ah,9
                int     21h                                     ; DOS Services  ah=function 09h
                                                                ;  display char string at ds:dx
                mov     ax,4C01h
                int     21h                                     ; DOS Services  ah=function 4Ch
                                                                ;  terminate with al=return code
loc_75:
                lodsb                                           ; String [si] to al
                call    sub_18                                  ; (05EA)
                push    di
                mov     di,offset Option_table                  ; (79E2:0B45=31h)
                mov     cx,0Dh
                repne   scasb                                   ; Rep zf=0+cx >0 Scan es:[di] for al
                jnz     loc_73                                  ; Jump if not zero
                sub     di,0B46h
                shl     di,1                                    ; Shift w/zeros fill
                clc                                             ; Clear carry flag
                call    word ptr data_17[di]                    ;*(79E2:0B52=0B6Ch)  13 entries
                jc      loc_73                                  ; Jump if carry Set
                pop     di
                jmp     short loc_72                            ; (0A3F)
loc_76:
                cmp     di,data_40e                             ; (79E2:1994=0)
                ja      loc_77                                  ; Jump if above
                call    sub_23                                  ; (0B1E)
                jmp     short loc_78                            ; (0A9A)
loc_77:
                or      data_7,4                                ; (79E2:0117=0)
loc_78:
                mov     al,5Ch                                  ; '\'
                cmp     al,[di-1]
                je      loc_79                                  ; Jump if equal
                stosb                                           ; Store al to es:[di]
loc_79:
                mov     al,0
                stosb                                           ; Store al to es:[di]
                mov     dx,789h
                cmp     word ptr ds:data_41e,5C3Ah              ; (79E2:1995=0)
                jne     loc_74                                  ; Jump if not equal
                mov     dx,offset copy_right                    ; (79E2:07BE=50h)
                mov     ah,9
                int     21h                                     ; DOS Services  ah=function 09h
                                                                ;  display char string at ds:dx
                call    sub_21                                  ; (06BF)
                test    data_7,8                                ; (79E2:0117=0)
                jnz     loc_80                                  ; Jump if not zero
                jmp     loc_68                                  ; (06AD)
loc_80:
                mov     ax,3516h
                int     21h                                     ; DOS Services  ah=function 35h
                                                                ;  get intrpt vector al in es:bx
                mov     ds:Old_int_16h,bx                       ; (79E2:1976=0)
                mov     word ptr ds:Old_int_16h+2,es            ; (79E2:1978=79E2h)
                mov     dx,offset int_16h_entry
                mov     ax,2516h
                int     21h                                     ; DOS Services  ah=function 25h
                                                                ;  set intrpt vector al to ds:dx
                mov     ax,351Ch
                int     21h                                     ; DOS Services  ah=function 35h
                                                                ;  get intrpt vector al in es:bx
                mov     ds:Old_int_1Ch,bx                       ; (79E2:197A=0)
                mov     word ptr ds:Old_int_1Ch+2,es            ; (79E2:197C=79E2h)
                mov     dx,offset int_1Ch_entry
                mov     ax,251Ch
                int     21h                                     ; DOS Services  ah=function 25h
                                                                ;  set intrpt vector al to ds:dx
                mov     ax,3528h
                int     21h                                     ; DOS Services  ah=function 35h
                                                                ;  get intrpt vector al in es:bx
                mov     ds:Old_int_28h,bx                       ; (79E2:197E=0)
                mov     word ptr ds:Old_int_28h+2,es            ; (79E2:1980=79E2h)
                mov     dx,offset int_28h_entry
                mov     ax,2528h
                int     21h                                     ; DOS Services  ah=function 25h
                                                                ;  set intrpt vector al to ds:dx
                mov     ah,34h                                  ; '4'
                int     21h                                     ; DOS Services  ah=function 34h
                                                                ;  get DOS critical ptr es:bx
                mov     ds:data_36e,bx                          ; (79E2:1986=0)
                mov     word ptr ds:data_36e+2,es               ; (79E2:1988=79E2h)
                mov     bx,4
loc_81:
                mov     ah,3Eh                                  ; '>'
                int     21h                                     ; DOS Services  ah=function 3Eh
                                                                ;  close file, bx=file handle
                dec     bx
                jns     loc_81                                  ; Jump if not sign
                mov     dx,1B08h
                int     27h                                     ; Terminate & stay resident

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_23          proc    near
                mov     ds,ds:data_1e                           ; (79E2:002C=0)
                xor     si,si                                   ; Zero register
loc_82:
                lodsb                                           ; String [si] to al
                or      al,al                                   ; Zero ?
                jnz     loc_82                                  ; Jump if not zero
                lodsb                                           ; String [si] to al
                or      al,al                                   ; Zero ?
                jnz     loc_82                                  ; Jump if not zero
                lodsw                                           ; String [si] to ax
                cmp     ax,1
                jne     loc_85                                  ; Jump if not equal
loc_83:
                lodsb                                           ; String [si] to al
                stosb                                           ; Store al to es:[di]
                or      al,al                                   ; Zero ?
                jnz     loc_83                                  ; Jump if not zero
                push    es
                pop     ds
loc_84:
                dec     di
                cmp     byte ptr [di],5Ch                       ; '\'
                jne     loc_84                                  ; Jump if not equal
loc_85:
                push    es
                pop     ds
                retn
sub_23          endp

Option_table    db      '1234ABCDHIRS'
                db      21h
data_17         dw      offset SET_COM1                         ; Data table (indexed access) (0B6C)
data_18         dw      offset SET_COM2                         ; (0B78)
data_19         dw      offset SET_COM3                         ; (0B84)
data_20         dw      offset SET_COM4                         ; (0B90)
data_21         dw      offset SET_HALFTIMER                    ; (0B9C)
data_22         dw      offset SET_SHOW_BIN                     ; (0BA2)
data_23         dw      offset SET_ADD_CRLF                     ; (0BA8)
data_24         dw      offset SET_LINExxx                      ; (0BAE)
data_25         dw      offset SET_NO_HDR_SAVE                  ; (0BB4)
data_26         dw      offset SET_SHOW_INFOBL                  ; (0BBA)
data_27         dw      offset SET_MAKE_TSR                     ; (0BC0)
data_28         dw      offset sub_24                           ; (0BC6)
data_29         dw      offset sub_25                           ; (0BFB)

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

SET_COM1        proc    near
                mov     COMM_Base,3F8h                          ; (79E2:0104=3F8h)
                mov     IRQ_nr,4                                ; (79E2:0103=4)
                retn
SET_COM1        endp


;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

SET_COM2        proc    near
                mov     COMM_Base,2F8h                          ; (79E2:0104=3F8h)
                mov     IRQ_nr,3                                ; (79E2:0103=4)
                retn
SET_COM2        endp


;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

SET_COM3        proc    near
                mov     COMM_Base,3E8h                          ; (79E2:0104=3F8h)
                mov     IRQ_nr,2                                ; (79E2:0103=4)
                retn
SET_COM3        endp


;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

SET_COM4        proc    near
                mov     COMM_Base,2E8h                          ; (79E2:0104=3F8h)
                mov     IRQ_nr,5                                ; (79E2:0103=4)
                retn
SET_COM4        endp


;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

SET_HALFTIMER   proc    near
                shr     Bit_len,1                               ; (79E2:0106=7C4h) Shift w/zeros fill
                clc                                             ; Clear carry flag
                retn
SET_HALFTIMER   endp


;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

SET_SHOW_BIN    proc    near
                or      data_7,10h                              ; (79E2:0117=0)
                retn
SET_SHOW_BIN    endp


;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

SET_ADD_CRLF    proc    near
                or      data_7,40h                              ; (79E2:0117=0) '@'
                retn
SET_ADD_CRLF    endp


;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

SET_LINExxx     proc    near
                or      data_7,2                                ; (79E2:0117=0)
                retn
SET_LINExxx     endp


;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

SET_NO_HDR_SAVE proc    near
                or      data_7,20h                              ; (79E2:0117=0) ' '
                retn
SET_NO_HDR_SAVE endp


;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;         BLBLsub_35
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

SET_SHOW_INFOBL proc    near
                or      data_7,1                                ; (79E2:0117=0)
                retn
SET_SHOW_INFOBL endp


;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

SET_MAKE_TSR    proc    near
                or      data_7,0Ch                              ; (79E2:0117=0)
                retn
SET_MAKE_TSR    endp


;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_24          proc    near
                mov     di,1A00h
                or      data_7,4                                ; (79E2:0117=0)
loc_86:
                lodsb                                           ; String [si] to al
                call    sub_18                                  ; (05EA)
                cmp     al,2Fh                                  ; '/'
                je      loc_88                                  ; Jump if equal
                cmp     al,20h                                  ; ' '
                jbe     loc_88                                  ; Jump if below or =
                cmp     di,data_44e                             ; (79E2:1A07=0)
                ja      loc_89                                  ; Jump if above
                cmp     al,30h                                  ; '0'
                jb      loc_89                                  ; Jump if below
                cmp     al,5Ah                                  ; 'Z'
                ja      loc_89                                  ; Jump if above
                cmp     al,41h                                  ; 'A'
                jae     loc_87                                  ; Jump if above or =
                cmp     al,39h                                  ; '9'
                ja      loc_89                                  ; Jump if above
loc_87:
                stosb                                           ; Store al to es:[di]
                jmp     short loc_86                            ; (0BCE)
loc_88:
                dec     si
                mov     al,0
                stosb                                           ; Store al to es:[di]
                clc                                             ; Clear carry flag
                retn
loc_89:
                stc                                             ; Set carry flag
                retn
sub_24          endp


;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;                              SUBROUTINE
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

sub_25          proc    near
                or      data_7,80h                              ; (79E2:0117=0)
                retn
sub_25          endp


seg_a           ends



                end     start
