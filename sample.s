	.org $0600

	ldx #00
rsta:
	lda #00
	sta $00
draw:
	sta $0200,X
	inx
	inc $00
	lda $00
	cmp #$10
	bne draw
	cpx #$F0
	bcc rsta
	brk
