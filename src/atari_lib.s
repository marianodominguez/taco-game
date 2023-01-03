
;
; Mariano Domínguez
; 2022-12-4
;
; atari lib
;
        .include "atari.inc"

        .export         _cpeekc
        .export         _grmode
        .export         _sound
        .import         popa

; this implementation is broken. it returns always 0

_cpeekc:             ; Get character
       ldx #$60      ; The screen 
       lda #7        ; Get record
       sta ICCOM,x   ; Command byte
       lda #0        ; Special case of
       sta ICBLL,x   ; data transfer
       sta ICBLH,x   ; in accumulator
       jsr CIOV      ; Do the LOCATE
       rts           ; All done

; graphics mode opens the channel. _graphics(mode) cc65 is broken too

_grmode:
       ;jsr     popa   ;get mode
       PHA           ; Store on stack
       LDX #$60      ; IOCB6 for screen
       LDA #$C       ; CLOSE command
       STA ICCOM,X   ; in command byte
       JSR CIOV      ; Do the CLOSE
       LDX #$60      ; The screen again
       LDA #3        ; OPEN command
       STA ICCOM,X   ; in command byte
       LDA #<NAME    ; Name is "S:"
       STA ICBAL,X   ; Low byte
       LDA #>NAME    ; High byte
       STA ICBAH,X
       PLA           ; Get GRAPHICS n
       STA ICAX2,X   ; Graphics mode
       AND #$F0      ; Get high 4 bits
       EOR #$10      ; Flip high bit
       ORA #$C       ; Read or write
       STA ICAX1,X   ; n+16, n+32 etc.
       JSR CIOV      ; Setup GRAPHICS n
       RTS           ; All done

; play sound, arguments: voice, pitch, distortion, volume
.proc  _sound
       sta STORE2    ;save volume
       jsr popa      ;get distortion
       sta STORE1    ;save distortion 
       jsr popa      ;get pitch
       pha           ;save in stack
       jsr popa      ;get voice
       
       asl a         ;adjust voice *2 for offset in x
       tax 
       pla           ;get pitch from stack
       sta AUDF1,x ; store pitch
       
       lda #0
       sta AUDCTL
       lda #3
       stx SKCTL  ; init sound

       lda STORE1    ;get distortion
       asl a         ;ignore the high nibble
       asl a 
       asl a 
       asl a
       clc           ; setup for adding volume 
       adc STORE2    ; add volume
       sta AUDC1,x   ; volume + distortion in control channel
       rts
.endproc

; print a single byte in screen+y_register, restores x and accumulator
__debug:
       sta tval
       txa 
       pha ; save x
       lda tval
       adc #16  ;use icode for numbers 0-9
       clc
       sta (SAVMSC),y
       pla 
       tax ; restore x
       lda tval
       rts

NAME:   .byte "S:",$0 ; screen S:
           .bss
STORE1:    .res    1
STORE2:    .res    1
tval:    .res    1