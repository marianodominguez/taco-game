
;
; Mariano Domínguez
; 2022-12-4
;
; atari lib
;
        .include "atari.inc"

        .export         _cpeekc
        .export         _grmode
        .import         popa

_cpeekc:             ; Get character
       ldx #$60      ; The screen 
       lda #7        ; Get record
       sta ICCOM,x   ; Command byte
       lda #0        ; Special case of
       sta ICBLL,x   ; data transfer
       sta ICBLH,x   ; in accumulator
       jsr CIOV      ; Do the LOCATE
       rts           ; All done

_grmode:
       jsr     popa   ;get mode
       PHA           ; Store on stack
       LDX #$60      ; IOCB6 for screen
       LDA #$C       ; CLOSE command
       STA ICCOM,X   ; in command byte
       JSR CIOV      ; Do the CLOSE
       LDX #$60      ; The screen again
       LDA #3        ; OPEN command
       STA ICCOM,X   ; in command byte
       LDA #>NAME    ; Name is "S:"
       STA ICBAL,X   ; Low byte
       LDA #<NAME    ; High byte
       STA ICBAH,X
       PLA           ; Get GRAPHICS n
       STA ICAX2,X   ; Graphics mode
       AND #$F0      ; Get high 4 bits
       EOR #$10      ; Flip high bit
       ORA #$C       ; Read or write
       STA ICAX1,X   ; n+16, n+32 etc.
       JSR CIOV      ; Setup GRAPHICS n
       RTS           ; All done

NAME:   .byte "S:",$9B ; screen S: