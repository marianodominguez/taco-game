    .export      _play_sfx
	.import      popa
;
STEREOMODE	= 0				;0 => compile RMTplayer for mono 4 tracks
;								;1 => compile RMTplayer for stereo 8 tracks
;								;2 => compile RMTplayer for 4 tracks stereo L1 R2 R3 L4
;								;3 => compile RMTplayer for 4 tracks stereo L1 L2 R3 R4
;
;
	.include "rmtplayr.s"			;include RMT player routine
;
;
	.incbin "resources/song.rmt"				;include music RMT module
;
;
MODUL	= $4000				;address of RMT module
KEY		= $2fc				;keypressed code
;
	org $6000

_play_sfx:
	lda #0						;#0 => maximal volume initialization
	sta RMTGLOBALVOLUMEFADE
;
	ldx #<MODUL					;low byte of RMT module to X reg
	ldy #>MODUL					;hi byte of RMT module to Y reg
	lda #0						;starting song line 0-255 to A reg
	jsr RASTERMUSICTRACKER		;Init
;
	ldy #<vbi
	ldx #>vbi
	lda #$07
	jsr $e45c					;Start VBI routine
;
    rts
vbi:
;
	jsr RASTERMUSICTRACKER+3	;1 play
	jmp $e462					;end vbi
	run start					;run addr
;
