enum {
	REG_A,
	REG_X,
	REG_Y,
	REG_PC,
	REG_STACK,
	REG_PSTATE
};

enum {
	PSTATE_CARRY = 1 << 0,
	PSTATE_ZERO = 1 << 1,
	PSTATE_DECIMAL = 1 << 3,
	PSTATE_OVERFLOW = 1 << 6,
	PSTATE_NEGATIVE = 1 << 7
};

enum {
	INSTRUCTION_LDA_IMMED = 0xa9,
	INSTRUCTION_LDA_ZP = 0xa5,
	INSTRUCTION_STA_ABS = 0x8d,
	INSTRUCTION_STA_ZP = 0x85,
	INSTRUCTION_STA_ABS_X = 0x9d,
	INSTRUCTION_CMP_IMMED = 0xc9,
	INSTRUCTION_LDX = 0xa2,
	INSTRUCTION_STX = 0x8e,
	INSTRUCTION_INX = 0xe8,
	INSTRUCTION_DEX = 0xca,
	INSTRUCTION_TAX = 0xaa,
	INSTRUCTION_CPX_IMMED = 0xe0,
	INSTRUCTION_ADC_IMMED = 0x69,
	INSTRUCTION_ADC_ZP = 0x65,
	INSTRUCTION_INC_ZP = 0xe6,
	INSTRUCTION_BNE = 0xd0,
	INSTRUCTION_BCS = 0xb0,
	INSTRUCTION_BCC = 0x90,
	INSTRUCTION_JSR = 0x20,
	INSTRUCTION_RTS = 0x60,
	INSTRUCTION_LDA_ABS_X = 0xbd,
	INSTRUCTION_BEQ = 0xf0,
	INSTRUCTION_JMP = 0x4c,

	INSTRUCTION_BRK = 0x00
};

const int REG_COUNT = 5;
