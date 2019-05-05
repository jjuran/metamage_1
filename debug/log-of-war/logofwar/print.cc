/*
	print.cc
	--------
*/

#include "logofwar/print.hh"

// POSIX
#include <unistd.h>

// Standard C
#include <string.h>

// gear
#include "gear/hexadecimal.hh"
#include "gear/inscribe_decimal.hh"


namespace logofwar
{

static char buffer[ 128 ];

static short n_bytes_used;

static
void flush()
{
	if ( n_bytes_used )
	{
		write( STDERR_FILENO, buffer, n_bytes_used );
		
		n_bytes_used = 0;
	}
}

static
void queue( const char* s, unsigned n )
{
	const bool flushing = n == 0  ||  s[ n - 1 ] == '\n';
	
	if ( flushing  ||  n_bytes_used + n > sizeof buffer )
	{
		flush();
	}
	
	if ( flushing  ||  n_bytes_used >= sizeof buffer )
	{
		write( STDERR_FILENO, s, n );
	}
	else
	{
		memcpy( buffer + n_bytes_used, s, n );
		
		n_bytes_used += n;
	}
}

void print( const char* s, unsigned n )
{
	queue( s, n );
}

void print( const char* s )
{
	print( s, strlen( s ) );
}

void print_dec( int x )
{
	print( gear::inscribe_decimal( x ) );
}

void print_hex( int x )
{
	unsigned short n = gear::hexadecimal_magnitude( x );
	
	char buffer[ 8 ];
	
	gear::inscribe_n_hex_digits( buffer, x, n );
	
	print( buffer, n );
}

}
