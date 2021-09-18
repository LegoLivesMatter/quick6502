/* Copyright (C) 2021 Duje Mihanović
 *
 * This file is part of Quick6502.
 *
 * Quick6502 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * Quick6502 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with Quick6502. If not, see <https://www.gnu.org/licenses/>.
*/

enum {
	REG_A,
	REG_X,
	REG_Y,
	REG_PC, /* program counter */
	REG_STACK, /* stack pointer */
	REG_PSTATE /* processor state */
};

enum processor_state {
	PSTATE_CARRY = 1 << 0,
	PSTATE_ZERO = 1 << 1,
	PSTATE_DECIMAL = 1 << 3,
	PSTATE_OVERFLOW = 1 << 6,
	PSTATE_NEGATIVE = 1 << 7
};

enum instructions {
	/* Arithmetic operations */
	INSTRUCTION_ADC_IMMED = 0x69, /* ADd with Carry (immediate) */
	INSTRUCTION_ADC_ZP = 0x65, /* ADd with Carry (zero page) */
	INSTRUCTION_CMP_IMMED = 0xc9, /* CoMPare accumulator with address */
	INSTRUCTION_CPX_IMMED = 0xe0, /* ComPare register X with address */

	/* Branching instructions */
	INSTRUCTION_BCC = 0x90, /* Branch if Carry Clear */
	INSTRUCTION_BCS = 0xb0, /* Branch if Carry Set */
	INSTRUCTION_BEQ = 0xf0, /* Branch if EQual */
	INSTRUCTION_BNE = 0xd0, /* Branch if Not Equal */
	INSTRUCTION_JMP = 0x4c, /* JuMP to address */
	INSTRUCTION_JSR = 0x20, /* Jump to SubRoutine */
	INSTRUCTION_RTS = 0x60, /* ReTurn from Subroutine */

	/* Register operations */
	INSTRUCTION_DEX = 0xca, /* DEcrement X */
	INSTRUCTION_INX = 0xe8, /* INcrement X */
	INSTRUCTION_LDA_ABS_X = 0xbd, /* LoaD value into A (absolute,X) */
	INSTRUCTION_LDA_IMMED = 0xa9, /* LoaD value into A (immediate) */
	INSTRUCTION_LDA_ZP = 0xa5, /* LoaD value into A (zero page) */
	INSTRUCTION_LDX = 0xa2, /* LoaD value into X */
	INSTRUCTION_STA_ABS = 0x8d, /* STore A into address (absolute) */
	INSTRUCTION_STA_ABS_X = 0x9d, /* STore A into address (absolute,X) */
	INSTRUCTION_STA_ZP = 0x85, /* STore A into address (zero page) */
	INSTRUCTION_STX = 0x8e, /* STore X into address */
	INSTRUCTION_TAX = 0xaa, /* Transfer A to X */
	INSTRUCTION_TAY = 0xa8, /* Transfer A to Y */
	INSTRUCTION_TSX = 0xba, /* Transfer Stack pointer to X */
	INSTRUCTION_TXA = 0x8a, /* Transfer X to Accumulator */
	INSTRUCTION_TXS = 0x9a, /* Transfer X to Stack pointer */
	INSTRUCTION_TYA = 0x98, /* Transfer Y to Accumulator */
	
	/* Miscellaneous */
	INSTRUCTION_INC_ZP = 0xe6, /* INCrement value at address (zero page) */

	INSTRUCTION_BRK = 0x00
};

const int REG_COUNT = 5;
