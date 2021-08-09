	.org $8000
	
	ldx #$00
loop:
	lda text,x
	beq stop
	sta $200,x
	inx
	jmp loop
stop:
	brk
text:
	.string "Hello World!"
