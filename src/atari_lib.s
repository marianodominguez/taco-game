
;
; Mariano Domínguez
; 2022-12-4
;
; atari lib
;
        .include "atari.inc"
        .export         _sound
        .import         popa

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