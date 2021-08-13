#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <fcntl.h>

#include "enums.h"
#include "framebuffer.h"

#ifdef ENABLE_DEBUG
#define DEBUG(MSG) printf( "DEBUG: %s\n", MSG )
#else
#define DEBUG(MSG) 
#endif

#define ENDIAN_SWAP(LOW,HIGH) (uint16_t) ( LOW | ( HIGH << 8 ) )
#define SET_FLAG(PSTATE,FLAG) PSTATE |= FLAG
#define IS_FLAG_SET(PSTATE,FLAG) ((PSTATE) & (FLAG)) == (FLAG)
#define CLEAR_FLAG(PSTATE,FLAG) PSTATE &= ~(FLAG)

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
		case INSTRUCTION_LDA_IMMED: 
			registers[REG_A] = memory[temp_pc++];
			if( registers[REG_A] == 0 ) { SET_FLAG( registers[REG_PSTATE], PSTATE_ZERO ); } else { CLEAR_FLAG( registers[REG_PSTATE], PSTATE_ZERO ); }
			if( registers[REG_A] >= 0x80 ) { SET_FLAG( registers[REG_PSTATE], PSTATE_NEGATIVE ); } else { CLEAR_FLAG( registers[REG_PSTATE], PSTATE_NEGATIVE ); }
			state = 1;
			break;
		case INSTRUCTION_LDA_ZP: {
			registers[REG_A] = memory[temp_pc++];
			if( registers[REG_A] == 0 ) { SET_FLAG( registers[REG_PSTATE], PSTATE_ZERO ); } else { CLEAR_FLAG( registers[REG_PSTATE], PSTATE_ZERO ); }
			if( registers[REG_A] >= 0x80 ) { SET_FLAG( registers[REG_PSTATE], PSTATE_NEGATIVE ); } else { CLEAR_FLAG( registers[REG_PSTATE], PSTATE_NEGATIVE ); }
			state = 1;
			break;
		}
		case INSTRUCTION_STA_ABS: {
			uint8_t low_half = memory[temp_pc++];
			uint8_t high_half = memory[temp_pc++];
			uint16_t address = ENDIAN_SWAP( low_half, high_half );
			memory[address] = registers[REG_A];
			state = 1;
			break;
		}
		case INSTRUCTION_STA_ZP: {
			uint8_t address = memory[temp_pc++];
			memory[address] = registers[REG_A];
			state = 1;
			break;
		}
		case INSTRUCTION_STA_ABS_X: {
			uint8_t low_half = memory[temp_pc++];
			uint8_t high_half = memory[temp_pc++];
			uint16_t address = ENDIAN_SWAP( low_half, high_half ) + registers[REG_X];
			memory[address] = registers[REG_A];
			state = 1;
			break;
		}
		case INSTRUCTION_CMP_IMMED: {
			uint8_t a = registers[REG_A];
			uint8_t value = memory[temp_pc++];

			if( a >= value ) { SET_FLAG( registers[REG_PSTATE], PSTATE_CARRY ); } else { CLEAR_FLAG( registers[REG_PSTATE], PSTATE_CARRY ); }
			if( a == value ) { SET_FLAG( registers[REG_PSTATE], PSTATE_ZERO ); } else { CLEAR_FLAG( registers[REG_PSTATE], PSTATE_ZERO ); }
			if( a >= 0x80 ) { SET_FLAG( registers[REG_PSTATE], PSTATE_NEGATIVE ); } else { CLEAR_FLAG( registers[REG_PSTATE], PSTATE_NEGATIVE ); }
			
			state = 1;
			break;
		}
		case INSTRUCTION_LDX:
			registers[REG_X] = memory[temp_pc++];
			if( registers[REG_X] == 0 ) { SET_FLAG( registers[REG_PSTATE], PSTATE_ZERO ); } else { CLEAR_FLAG( registers[REG_PSTATE], PSTATE_ZERO ); }
			if( registers[REG_X] >= 0x80 ) { SET_FLAG( registers[REG_PSTATE], PSTATE_NEGATIVE ); } else { CLEAR_FLAG( registers[REG_PSTATE], PSTATE_NEGATIVE ); }
			state = 1;
			break;
		case INSTRUCTION_STX: {
			uint8_t low_half = memory[temp_pc];
			uint8_t high_half = memory[temp_pc + 1];
			uint16_t address = (uint16_t) ( low_half | ( high_half << 8 ) );
			memory[address] = registers[REG_X];
			temp_pc += 2;
			state = 1;
			break;
		}
		case INSTRUCTION_TAX:
			registers[REG_X] = registers[REG_A];
			state = 1;
			break;
		case INSTRUCTION_INX:
			registers[REG_X]++;
			state = 1;
			break;
		case INSTRUCTION_DEX:
			registers[REG_X]--;
			state = 1;
			break;
		case INSTRUCTION_CPX_IMMED: {
			uint8_t x = registers[REG_X];
			uint8_t value = memory[temp_pc++];

			if( x >= value ) { SET_FLAG( registers[REG_PSTATE], PSTATE_CARRY ); } else { CLEAR_FLAG( registers[REG_PSTATE], PSTATE_CARRY ); }
			if( x == value ) { SET_FLAG( registers[REG_PSTATE], PSTATE_ZERO ); } else { CLEAR_FLAG( registers[REG_PSTATE], PSTATE_ZERO ); }
			if( x >= 0x80 ) { SET_FLAG( registers[REG_PSTATE], PSTATE_NEGATIVE ); } else { CLEAR_FLAG( registers[REG_PSTATE], PSTATE_NEGATIVE ); }

			state = 1;
			break;
		}
		case INSTRUCTION_ADC_IMMED: {
			uint16_t sum = memory[temp_pc++] + registers[REG_A];
			if( sum > 0xFF ) { SET_FLAG( registers[REG_PSTATE], PSTATE_CARRY ); } else { CLEAR_FLAG( registers[REG_PSTATE], PSTATE_CARRY ); }
			registers[REG_A] = (uint8_t) sum;
			state = 1;
			break;
		}
		case INSTRUCTION_ADC_ZP: {
			uint8_t address = memory[temp_pc++];
			uint16_t sum = memory[address] + registers[REG_A];
			if( sum > 0xFF ) { SET_FLAG( registers[REG_PSTATE], PSTATE_CARRY ); } else { CLEAR_FLAG( registers[REG_PSTATE], PSTATE_CARRY ); }
			registers[REG_A] = (uint8_t) sum;
			state = 1;
			break;
		}
		case INSTRUCTION_INC_ZP: {
			uint8_t address = memory[temp_pc++];
			memory[address]++;
			state = 1;
			break;
		}
		case INSTRUCTION_BRK: {
			printf( "Execution complete\n" );
			state = 0;
			break;
		}
		case INSTRUCTION_BNE: {
			int8_t offset = (int8_t) memory[temp_pc++];
			if( !(IS_FLAG_SET( registers[REG_PSTATE], PSTATE_ZERO ) ) ) {
				temp_pc += offset;
			}
			state = 1;
			break;
		}
		case INSTRUCTION_BCS: {
			int8_t offset = (int8_t) memory[temp_pc++];
			if( IS_FLAG_SET( registers[REG_PSTATE], PSTATE_CARRY ) ) {
				temp_pc += offset;
			}
			state = 1;
			break;
		}
		case INSTRUCTION_BCC: {
			int8_t offset = (int8_t) memory[temp_pc++];
			if( !(IS_FLAG_SET( registers[REG_PSTATE], PSTATE_CARRY ) ) ) {
				temp_pc += offset;
			}
			state = 1;
			break;
		}
		case INSTRUCTION_JSR: {
			uint8_t oldpclow, oldpchigh, temp_sp = registers[REG_STACK];
			uint16_t decpc = ++temp_pc;
			oldpclow = decpc & 0xff;
			oldpchigh = decpc >> 8;

			memory[0x100 | temp_sp--] = oldpchigh;
			memory[0x100 | temp_sp--] = oldpclow;

			registers[REG_STACK] = temp_sp;

			uint8_t pclow, pchigh;
			pclow = memory[--temp_pc];
			pchigh = memory[++temp_pc];
			temp_pc = ENDIAN_SWAP( pclow, pchigh );
			state = 1;
			break;
		}
		case INSTRUCTION_RTS: {
			uint8_t returnlow, returnhigh, temp_sp = registers[REG_STACK];
			uint16_t returnpc;

			returnlow = memory[0x100 | temp_sp++];
			returnhigh = memory[0x100 | temp_sp++];
			returnpc = ENDIAN_SWAP( returnlow, returnhigh) + 1;

			registers[REG_STACK] = temp_sp;
			temp_pc = returnpc;
			state = 1;
			break;
		}
		case INSTRUCTION_LDA_ABS_X: {
			uint8_t addrlow, addrhigh;
			uint16_t addr;

			addrlow = memory[temp_pc++];
			addrhigh = memory[temp_pc++];
			addr = ENDIAN_SWAP( addrlow, addrhigh );

			registers[REG_A] = memory[addr + registers[REG_X]];
			if( registers[REG_A] == 0 ) { SET_FLAG( registers[REG_PSTATE], PSTATE_ZERO ); } else { CLEAR_FLAG( registers[REG_PSTATE], PSTATE_ZERO ); }
			if( registers[REG_A] >= 0x80 ) { SET_FLAG( registers[REG_PSTATE], PSTATE_NEGATIVE ); } else { CLEAR_FLAG( registers[REG_PSTATE], PSTATE_NEGATIVE ); }
			state = 1;
			break;
		}
		case INSTRUCTION_BEQ: {
			int8_t offset;

			if( IS_FLAG_SET( registers[REG_PSTATE], PSTATE_ZERO) ) {
				offset = (int8_t) memory[temp_pc++];
				temp_pc += offset;
			} else { temp_pc++; }

			state = 1;
			break;
		}
		case INSTRUCTION_JMP: {
			uint8_t lowaddr, highaddr;
			uint16_t newpc;

			lowaddr = memory[temp_pc++];
			highaddr = memory[temp_pc++];
			newpc = ENDIAN_SWAP( lowaddr, highaddr );
			temp_pc = newpc;

			state = 1;
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

	/* Write default reset vector, 0x8000 */
	memory[ 0xFFFC ] = 0x00;
	memory[ 0xFFFD ] = 0x80;

	uint8_t *registers = calloc( REG_COUNT, sizeof( uint8_t ) );
	registers[REG_STACK] = 0xff;

	if( argv[1] == NULL ) {
		printf( "Please specify a 6502 program to run!\n" );
		return 2;
	}
	
	DEBUG( "Opening program" );

	FILE *program = fopen( argv[1], "rb" );

	if( program == NULL ) {
		printf( "Failed to open program, does the program exist and do you have permissions to open the program?\n" );
		return 2;
	}

	DEBUG( "Loading program" );

	long program_size = 0;

	if( fseek( program, 0L, SEEK_END ) ) {
		printf( "Failed to get program size!\n" );
		fclose( program );
		return 1;
	}

	program_size = ftell( program );

	if( program_size > 65023 ) { 
		printf( "Program size exceeds ~64K, cannot continue!\n" );
		fclose( program );
		return 1;
	}

	rewind( program );

	fread( memory + 0x8000, sizeof( uint8_t ), program_size, program );

	if( fclose( program ) ) {
		printf( "Failed to close program file, aborting!\n" );
		return 1;
	}
	memory_dump( memory, "start_dump.bin" );

	uint16_t program_counter = ENDIAN_SWAP( memory[ 0xFFFC ], memory[ 0xFFFD ] );

	DEBUG("Initialize framebuffer");

	struct framebuffer *fb = NULL;
	if( init_framebuffer( &fb ) ) {
		printf( "Failed to initialize framebuffer!\n" );
		return 1;
	}

	printf( "Begin execution at %X\n", program_counter );

	uint8_t *prev_fbmem = calloc( 0x400, sizeof( uint8_t ) );
	const uint8_t *fbmem = memory + 0x200;

	int status = 1;
	while( status )
	{
		status = parse_instruction( memory, registers, &program_counter );
		update_framebuffer( fb, fbmem, prev_fbmem, 0x400 );
	}

	memory_dump( memory, "end_dump.bin" );

	printf( "Press Enter to close emulator\n" );
	getchar();
	free( prev_fbmem );
	free( memory );
	free( registers );
	destroy_framebuffer( fb );
	shutdown_framebuffer();

	return 0;
}
