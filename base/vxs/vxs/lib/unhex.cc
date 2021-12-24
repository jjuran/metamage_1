/*
	unhex.cc
	--------
*/

#include "vxs/lib/unhex.hh"

// iota
#include "iota/char_types.hh"

// gear
#include "gear/hexadecimal.hh"

// debug
#include "debug/assert.hh"


namespace vxo
{

Expected_String unhex( const char* data, size_t size, align_t align )
{
	// Alignment must be a power of two.
	
	ASSERT( align != 0 );
	
	const unsigned mask = align - 1;
	
	ASSERT( (align ^ mask) == (align + mask) );
	
	for ( size_t i = 0;  i < size;  ++i )
	{
		if ( ! iota::is_xdigit( data[ i ] ) )
		{
			return Error( "invalid hexadecimal digit" );
		}
	}
	
	size_t n_data_bytes = size / 2 + (size & 1);
	size_t n_zero_bytes = mask - (n_data_bytes + mask & mask);
	size_t n_bytes      = n_data_bytes + n_zero_bytes;
	
	plus::string result;
	
	char* p = result.reset( n_bytes );
	
	while ( n_zero_bytes-- )
	{
		*p++ = 0;
	}
	
	size_t n = n_data_bytes;
	
	if ( const bool has_unpaired_digit = size & 1 )
	{
		*p++ = gear::decoded_hex_digit( *data++ );
		
		--n;
	}
	
	for ( ;  n > 0;  --n )
	{
		unsigned char c;
		
		c  = gear::decoded_hex_digit( *data++ ) << 4;
		c |= gear::decoded_hex_digit( *data++ );
		
		*p++ = c;
	}
	
	return String( result );
}

}
