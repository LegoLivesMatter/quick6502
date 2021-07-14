	LDX #00
rsta:
	LDA #00
	STA $00
draw:
	STA $0200,X
	INX
	INC $00
	LDA $00
	CMP #$10
	BNE draw
	CPX #$F0
	BCC rsta
	BRK
