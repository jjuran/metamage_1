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


static char hexify( unsigned x )
{
	x &= 0x0F;
	
	return (x <= 9 ? '0' : 'a' - 10) + x;
}

static void inscribe_16( char* p, uint16_t x )
{
	p[0] = hexify( x >> 12 );
	p[1] = hexify( x >>  8 );
	p[2] = hexify( x >>  4 );
	p[3] = hexify( x >>  0 );
}

static void inscribe_32( char* p, uint32_t x )
{
	p[0] = hexify( x >> 28 );
	p[1] = hexify( x >> 24 );
	p[2] = hexify( x >> 20 );
	p[3] = hexify( x >> 16 );
	p[4] = hexify( x >> 12 );
	p[5] = hexify( x >>  8 );
	p[6] = hexify( x >>  4 );
	p[7] = hexify( x >>  0 );
}

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

#define DUMP_TEMPLATE             "\n" \
	"D0: 12345678   A0: 12345678" "\n" \
	"D1: 12345678   A1: 12345678" "\n" \
	"D2: 12345678   A2: 12345678" "\n" \
	"D3: 12345678   A3: 12345678" "\n" \
	"D4: 12345678   A4: 12345678" "\n" \
	"D5: 12345678   A5: 12345678" "\n" \
	"D6: 12345678   A6: 12345678" "\n" \
	"D7: 12345678   A7: 12345678" "\n" \
	                              "\n" \
	"Alt SP:  12345678"           "\n" \
	"Alt SSP: 12345678"           "\n" \
	                              "\n" \
	"PC:  12345678"               "\n" \
	"SR:  1234"                   "\n" \
	                              "\n"

const size_t dump_size = sizeof DUMP_TEMPLATE - 1;

void print_register_dump( const uint32_t  d[],
                          const uint32_t  a[],
                          uint32_t        alt_sp,
                          uint32_t        alt_ssp,
                          uint32_t        pc,
                          uint16_t        sr )
{
	char dump[] = DUMP_TEMPLATE;
	
	char* p = dump;
	
	++p;  // skip newline
	
	for ( int i = 0;  i < 8;  ++i )
	{
		p += sizeof "Di: " - 1;
		
		inscribe_32( p, d[i] );
		
		p += sizeof "12345678   Ai: " - 1;
		
		inscribe_32( p, a[i] );
		
		p += sizeof "12345678" "\n" - 1;
	}
	
	++p;  // skip newline
	
	p += sizeof "Alt SP:  " - 1;
	
	inscribe_32( p, alt_sp );
	
	p += sizeof "12345678" "\n" - 1;
	
	p += sizeof "Alt SSP: " - 1;
	
	inscribe_32( p, alt_ssp );
	
	p += sizeof "12345678" "\n" - 1;
	
	++p;  // skip newline
	
	p += sizeof "PC:  " - 1;
	
	inscribe_32( p, pc );
	
	p += sizeof "12345678" "\n" - 1;
	
	p += sizeof "SR:  " - 1;
	
	inscribe_16( p, sr );
	
	write( STDOUT_FILENO, dump, dump_size );
}

