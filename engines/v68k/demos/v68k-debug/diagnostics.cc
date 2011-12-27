/*
	diagnostics.cc
	--------------
*/

#include "diagnostics.hh"

// Standard C
#include <stdio.h>


#pragma exceptions off


void err_allocation_failed( uint32_t n_bytes )
{
	fprintf( stderr, "Unable to allocate %d bytes\n", n_bytes );
}

void err_argv_too_big()
{
	fprintf( stderr, "Parameter area exceeded by argv\n" );
}

void err_argv_way_too_big()
{
	fprintf( stderr, "Parameter area well exceeded by argv\n" );
}

void note_program_loaded( uint32_t n_bytes, const char* path )
{
	fprintf( stderr, "Loaded %d bytes from %s\n", n_bytes, path );
}

void print_blank_line()
{
	putchar( '\n' );
}

void print_instruction_limit_exceeded( int limit )
{
	printf( "%d instruction limit exceeded\n", limit );
}

void print_halted()
{
	printf( "%s\n", "Halted" );
}

void print_stopped()
{
	printf( "%s\n", "Stopped" );
}

void print_breakpoint( int number )
{
	printf( "Breakpoint %d\n", number );
}

void print_register_dump( const uint32_t  d[],
                          const uint32_t  a[],
                          uint32_t        alt_sp,
                          uint32_t        alt_ssp,
                          uint32_t        pc,
                          uint16_t        sr )
{
	print_blank_line();
	
	for ( int i = 0;  i < 8;  ++i )
	{
		printf( "D%d: %.8x   A%d: %.8x\n", i, d[i], i, a[i] );
	}
	
	print_blank_line();
	
	printf( "Alt SP:  %.8x\n", alt_sp  );
	printf( "Alt SSP: %.8x\n", alt_ssp );
	
	print_blank_line();
	
	printf( "PC: %.8x\n", pc );
	printf( "SR: %.4x\n", sr );
	
	print_blank_line();
}

