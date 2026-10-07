/*
	damogran-test-unpack.cc
	-----------------------
*/

// POSIX
#include <unistd.h>

// Standard C
#include <stdlib.h>
#include <string.h>

// gear
#include "gear/hexadecimal.hh"

// damogran
#include "damogran/unpack.hh"


#pragma exceptions off


typedef unsigned char Byte;

static
void test_unpack( char* hex, size_t n_hex )
{
	using damogran::unpack;
	using damogran::unpack_preflight;
	
	if ( n_hex == 0 )
	{
		write( STDOUT_FILENO, "\n", 1 );
		
		return;
	}
	
	size_t n_packed = n_hex / 2;
	
	Byte* packed = (Byte*) hex;
	
	for ( int i = 0;  i < n_packed;  ++i )
	{
		packed[ i ] = gear::decode_8_bit_hex( hex );
		
		hex += 2;
	}
	
	long n_unpacked = unpack_preflight( packed, packed + n_packed );
	
	if ( n_unpacked < 0 )
	{
		return;
	}
	
	if ( Byte* unpacked = (Byte*) malloc( n_unpacked * 3 + 1 ) )
	{
		Byte const* src = packed;
		Byte*       dst = unpacked;
		Byte*       end = unpacked + n_unpacked;
		
		const Byte* got = unpack( src, dst, end );
		
		hex = (char*) end;
		
		char* hex_end = gear::hexpcpy_lower( hex, unpacked, n_unpacked );
		
		*hex_end = '\n';
		
		write( STDOUT_FILENO, hex, n_unpacked * 2 + 1 );
		
		free( unpacked );
	}
}

int main( int argc, char** argv )
{
	char** args = argv + 1;
	
	while ( char* arg = *args++ )
	{
		test_unpack( arg, strlen( arg ) );
	}
	
	return 0;
}
