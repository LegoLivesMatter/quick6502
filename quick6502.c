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

int main( int argc, char** argv ) {
	uint8_t register_a, register_x, register_y;
	uint16_t *program_counter = 0x0;

	uint8_t *memory = calloc( UINT16_MAX, sizeof( uint8_t ) );

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

	uint16_t temp_counter = (uint16_t) program_counter;
	int next_byte = fgetc( program );

	while( next_byte != EOF )
	{
		printf( "Writing 0x%x to %p\n", next_byte, memory + temp_counter );
		memset( memory + temp_counter, next_byte, 1 );
		next_byte = fgetc( program );
		temp_counter++;
	}

	debug( "Successfully loaded program\n" );

	return 0;
}
