/*
	diagnostics.cc
	--------------
*/

#include "diagnostics.hh"

// POSIX
#include <string.h>
#include <unistd.h>


#pragma exceptions off


#define OUT( msg )  ::write( STDOUT_FILENO, "" msg "\n", sizeof msg )
#define ERR( msg )  ::write( STDERR_FILENO, "" msg "\n", sizeof msg )


void err_allocation_failed( uint32_t n_bytes )
{
	ERR( "Out of memory" );
}

void err_argv_too_big()
{
	ERR( "Parameter area exceeded by argv" );
}

void err_argv_way_too_big()
{
	ERR( "Parameter area well exceeded by argv" );
}

void note_program_loaded( uint32_t n_bytes, const char* path )
{
	//fprintf( stderr, "Loaded %d bytes from %s\n", n_bytes, path );
}

void print_blank_line()
{
	OUT( "" );
}

void print_instruction_limit_exceeded( const char* limit )
{
	write( STDOUT_FILENO, limit, strlen( limit ) );
	
	OUT( " instruction limit exceeded" );
}

void print_halted()
{
	OUT( "Halted" );
}

void print_stopped()
{
	OUT( "Stopped" );
}

void print_breakpoint( int number )
{
	char msg[] = "Breakpoint 0\n";
	
	const int i = sizeof "Breakpoint " - 1;
	
	msg[ i ] += number;
	
	write( STDOUT_FILENO, msg, sizeof msg - 1 );
}

