
;
; Mariano Domínguez
; 2022-12-4
;
; atari lib
;
        .export      _sound
        .import      popa
        .importzp    tmp1,tmp2,tmp3

        .include "atari.inc"

; play sound, arguments: voice, pitch, distortion, volume
.proc  _sound
       and #15       ;ignore extra bits in volume
       sta tmp2      ;save volume, in accumulator by default

       jsr popa      ;get distortion
       sta tmp1      ;save distortion
       jsr popa      ;get pitch
       sta tmp3      ;save pitch in tmp3
       jsr popa      ;get voice

       asl a         ;adjust voice *2 for offset in x
       tax
       lda tmp3      ; get pitch
       sta AUDF1,x   ; store pitch

       lda #0
       sta AUDCTL
       lda #3
       sta SKCTL     ;init sound

       lda tmp1      ;get distortion
       asl a         ;ignore the high nibble
       asl a
       asl a
       asl a
       clc           ; setup for adding volume
       adc tmp2      ; add volume
       sta AUDC1,x   ; volume + distortion in control channel
       rts
.endproc

; print a single byte in screen+y_register, restores x and accumulator
__debug:
       sta tmp1
       txa
       pha ; save x
       lda tmp1
       adc #16  ;use icode for numbers 0-9
       clc
       sta (SAVMSC),y
       pla
       tax ; restore x
       lda tmp1
       rts
NAME:   .byte "S:",$0 ; screen S:
