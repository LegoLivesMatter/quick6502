#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "enums.h"
#include "framebuffer.h"

#define ENABLE_DEBUG

#ifdef ENABLE_DEBUG
#define DEBUG(MSG) printf( "DEBUG: %s\n", MSG )
#endif

#define ENDIAN_SWAP(LOW,HIGH) (uint16_t) ( LOW | ( HIGH << 8 ) )

char *nibble_lookup[] = {
	[0x0] = "0000",
	[0x1] = "0001",
	[0x2] = "0010",
	[0x3] = "0011",
	[0x4] = "0100",
	[0x5] = "0101",
	[0x6] = "0110",
	[0x7] = "0111",
	[0x8] = "1000",
	[0x9] = "1001",
	[0xa] = "1010",
	[0xb] = "1011",
	[0xc] = "1100",
	[0xd] = "1101",
	[0xe] = "1110",
	[0xf] = "1111"
};

char *get_bin( uint8_t number ) {
	char *bin = calloc( 9, sizeof( char ) );
	sprintf( bin, "%s%s", nibble_lookup[number >> 4], nibble_lookup[number & 0xf] );
	return bin;
}

int parse_instruction( uint8_t *memory, uint8_t *registers, uint16_t *pc ) {
	int state;
	uint16_t temp_pc = *pc;
	uint8_t instruction = memory[temp_pc];
	temp_pc++;
	switch( instruction ) {
		case INSTRUCTION_LDA: 
			registers[REG_A] = memory[temp_pc++];
			printf( "Got LDA, new value of A is 0x%x\n", registers[REG_A] );
			state = 1;
			break;
		case INSTRUCTION_STA_ABS: {
			uint8_t low_half = memory[temp_pc++];
			uint8_t high_half = memory[temp_pc++];
			uint16_t address = ENDIAN_SWAP( low_half, high_half );
			memory[address] = registers[REG_A];
			printf( "Got STA w/ absolute addressing, value of %p is now %x\n", address, registers[REG_A] );
			state = 1;
			break;
		}
		case INSTRUCTION_STA_ZP: {
			uint8_t address = memory[temp_pc++];
			memory[address] = registers[REG_A];
			printf( "Got STA w/ zero page addressing, value of %p is now 0x%x\n", address, registers[REG_A] );
			state = 1;
			break;
		}
		case INSTRUCTION_LDX:
			registers[REG_X] = memory[temp_pc++];
			printf( "Got LDX, new value of X is 0x%x\n", registers[REG_X] );
			state = 1;
			break;
		case INSTRUCTION_STX: {
			uint8_t low_half = memory[temp_pc];
			uint8_t high_half = memory[temp_pc + 1];
			uint16_t address = (uint16_t) ( low_half | ( high_half << 8 ) );
			memory[address] = registers[REG_X];
			temp_pc += 2;
			printf( "Got STX, value of %p is now %x\n", address, registers[REG_X] );
			state = 1;
			break;
		}
		case INSTRUCTION_TAX:
			registers[REG_X] = registers[REG_A];
			printf( "Got TXA, register X is now 0x%x\n", registers[REG_X] );
			state = 1;
			break;
		case INSTRUCTION_INX:
			registers[REG_X]++;
			printf( "Got INX, register X is now 0x%x\n", registers[REG_X] );
			state = 1;
			break;
		case INSTRUCTION_DEX:
			registers[REG_X]--;
			printf( "Got DEX, register X is now 0x%x\n", registers[REG_X] );
			state = 1;
			break;
		case INSTRUCTION_CPX_IMMED: {
			uint8_t x = registers[REG_X];
			uint8_t value = memory[temp_pc++];
			if( x >= value ) {
				registers[REG_PSTATE] |= PSTATE_CARRY;
			}

			if( x == value ) {
				registers[REG_PSTATE] |= PSTATE_ZERO;
			}

			if( x >= 0x80 ) {
				registers[REG_PSTATE] |= PSTATE_NEGATIVE;
			}

			printf( "Got CPX w/ immediate addressing, PSTATE is now %s\n", get_bin( registers[REG_PSTATE] ) );
			state = 1;
			break;
		}
		case INSTRUCTION_ADC_IMMED: {
			uint16_t sum = memory[temp_pc++] + registers[REG_A];
			if( sum > 0xFF ) {
				registers[REG_PSTATE] |= PSTATE_CARRY;
			}
			registers[REG_A] = (uint8_t) sum;
			printf( "Got ADC w/ immediate addressing, register A is now 0x%x\n", registers[REG_A] );
			state = 1;
			break;
		}
		case INSTRUCTION_ADC_ZP: {
			uint8_t address = memory[temp_pc++];
			uint16_t sum = memory[address] + registers[REG_A];
			if( sum > 0xFF ) {
				registers[REG_PSTATE] |= PSTATE_CARRY;
			}
			registers[REG_A] = (uint8_t) sum;
			printf( "Got ADC w/ zero page addressing, register A is now 0x%x\n", registers[REG_A] );
			state = 1;
			break;
		}
		case INSTRUCTION_BRK: {
			printf( "Execution complete\n" );
			state = 0;
			break;
		}
		default: 
			printf( "Unknown instruction 0x%x, check program or fix emulator!\n", instruction );
			state = 0;
			break;
	}

	*pc = temp_pc;
	return state;
}

int memory_dump( uint8_t *memory, char *filepath ) {
	FILE *dump = fopen( filepath, "wb" );

	if( !dump )
	{
		return 0;
	}

	fwrite( memory, sizeof( uint8_t ), UINT16_MAX, dump );

	if( fclose( dump ) )
	{
		return 0;
	}

	return 1;
}

int main( int argc, char** argv ) {
	uint8_t *memory = calloc( UINT16_MAX,  sizeof( uint8_t ) );

	/* Write default reset vector, 0x6600 */
	memory[ 0xFFFC ] = 0x00;
	memory[ 0xFFFD ] = 0x66;

	uint8_t *registers = calloc( REG_COUNT, sizeof( uint8_t ) );

	if( argv[1] == NULL ) {
		printf( "Please specify a 6502 program to run!\n" );
		return 2;
	}
	
	#ifdef ENABLE_DEBUG
	DEBUG( "Opening program" );
	#endif

	FILE *program = fopen( argv[1], "rb" );

	if( program == NULL ) {
		printf( "Failed to open program, does the program exist and do you have permissions to open the program?\n" );
		return 2;
	}

	#ifdef ENABLE_DEBUG
	DEBUG( "Loading program" );
	#endif

	long program_size = 0;

	if( fseek( program, 0L, SEEK_END ) ) {
		printf( "Failed to get program size!\n" );
		fclose( program );
		return 1;
	}

	program_size = ftell( program );

	if( program_size > 65023 ) { /* 65535 - 256 */
		printf( "Program size exceeds ~64K, cannot continue!\n" );
		fclose( program );
		return 1;
	}

	rewind( program );

	fread( memory + 0x6600, sizeof( uint8_t ), program_size, program );

	if( fclose( program ) ) {
		printf( "Failed to close program file, aborting!\n" );
		return 1;
	}

	memory_dump( memory, "start_dump.bin" );

	uint16_t program_counter = ENDIAN_SWAP( memory[ 0xFFFC ], memory[ 0xFFFD ] );

	printf( "Begin execution at %X\n", program_counter );

	int status = 1;
	while( status )
	{
		status = parse_instruction( memory, registers, &program_counter);
	}

	memory_dump( memory, "end_dump.bin" );

	return 0;
}
