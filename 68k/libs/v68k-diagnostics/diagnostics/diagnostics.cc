/*
	diagnostics.cc
	--------------
*/

#include "diagnostics/diagnostics.hh"

// POSIX
#include <string.h>
#include <unistd.h>


#ifdef __MWERKS__
#pragma exceptions off
#endif


#define ERR( msg )  ::write( STDERR_FILENO, "" msg "\n", sizeof msg )


namespace v68k        {
namespace diagnostics {

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

void print_blank_line()
{
	ERR( "" );
}

void print_instruction_limit_exceeded( const char* limit )
{
	write( STDERR_FILENO, limit, strlen( limit ) );
	
	ERR( " instruction limit exceeded" );
}

void print_halted()
{
	ERR( "Halted" );
}

void print_stopped()
{
	ERR( "Stopped" );
}

void print_breakpoint( int number )
{
	char msg[] = "Breakpoint 0\n";
	
	const int i = sizeof "Breakpoint " - 1;
	
	msg[ i ] += number;
	
	write( STDERR_FILENO, msg, sizeof msg - 1 );
}

}  // namespace diagnostics
}  // namespace v68k
