	LDA #00
	STA $00
	LDX #00
draw:
	STA $0200,X
	INX
	INC $00
	LDA $00
	CPX #$10
	BNE draw
	BRK
