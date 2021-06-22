#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "enums.h"

#define DEBUG

void debug( const char *message ) {
#ifdef DEBUG
	printf( "DEBUG: %s\n", message );
#endif
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
		case INSTRUCTION_STA: {
			uint8_t low_half = memory[temp_pc];
			uint8_t high_half = memory[temp_pc + 1];
			uint16_t address = (uint16_t) ( low_half | ( high_half << 8 ) );
			memory[address] = registers[REG_A];
			temp_pc += 2;
			printf( "Got STA, value of %p is now %x\n", address, registers[REG_A] );
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
		case 0x02: 
			printf( "Execution complete\n" );
			state = 0;
			break;
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
	uint8_t *memory = malloc( UINT16_MAX * sizeof( uint8_t ) );
	memset( memory, 0x02, UINT16_MAX );
	uint8_t *registers = calloc( REG_COUNT, sizeof( uint8_t ) );
	uint16_t program_counter = 0x200; /* 0x00-0xff is zero page, 0x100-0x1ff is stack */

	if( argv[1] == NULL ) {
		printf( "Please specify a 6502 program to run!\n" );
		return 2;
	}

	debug( "Opening program" );

	FILE *program = fopen( argv[1], "rb" );

	if( program == NULL ) {
		printf( "Failed to open program, does the program exist and do you have permissions to open the program?\n" );
		return 2;
	}

	debug( "Loading program" );

	long program_size = 0;

	if( fseek( program, 0L, SEEK_END ) ) {
		printf( "Failed to get program size!\n" );
		return 1;
	}

	program_size = ftell( program );

	if( program_size > 65278 ) {
		printf( "Program size exceeds ~64K, cannot continue!\n" );
		return 1;
	}

	rewind( program );
	fread( memory + program_counter, sizeof( uint8_t ), program_size, program );

	if( fclose( program ) ) {
		printf( "Failed to close program file, aborting!\n" );
		return 1;
	}

	memory_dump( memory, "start_dump.bin" );

	debug( "Begin execution" );

	int status = 1;
	while( status )
	{
		status = parse_instruction( memory, registers, &program_counter);
	}

	memory_dump( memory, "end_dump.bin" );

	return 0;
}
