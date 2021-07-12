enum {
	FLAG_NEG = 128,
	FLAG_OVERFLOW = 64,
	FLAG_BRK = 16,
	FLAG_DECIMAL = 8,
	FLAG_IRQB_DISABLE = 4,
	FLAG_ZERO = 2,
	FLAG_CARRY = 1
};

enum {
	REG_A,
	REG_X,
	REG_Y,
	REG_PC,
	REG_STACK,
	REG_PSTATE
};

enum {
	PSTATE_CARRY = 1,
	PSTATE_ZERO = 2,
	PSTATE_DECIMAL = 8,
	PSTATE_OVERFLOW = 64,
	PSTATE_NEGATIVE = 128
};

enum {
	INSTRUCTION_LDA_IMMED = 0xa9,
	INSTRUCTION_LDA_ZP = 0xa5,
	INSTRUCTION_STA_ABS = 0x8d,
	INSTRUCTION_STA_ZP = 0x85,
	INSTRUCTION_STA_ABS_X = 0x9d,
	INSTRUCTION_LDX = 0xa2,
	INSTRUCTION_STX = 0x8e,
	INSTRUCTION_INX = 0xe8,
	INSTRUCTION_DEX = 0xca,
	INSTRUCTION_TAX = 0xaa,
	INSTRUCTION_CPX_IMMED = 0xe0,
	INSTRUCTION_ADC_IMMED = 0x69,
	INSTRUCTION_ADC_ZP = 0x65,
	INSTRUCTION_INC_ZP = 0xe6,

	INSTRUCTION_BRK = 0x00
};

const int REG_COUNT = 5;
