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
	
	size_t n_digits = 0;
	size_t n_digits_since_space = 0;
	
	for ( size_t i = 0;  i < size;  ++i )
	{
		if ( ! iota::is_xdigit( data[ i ] ) )
		{
			if ( data[ i ] != ' ' )
			{
				return Error( "invalid hexadecimal digit" );
			}
			
			if ( n_digits_since_space % 2u != 0 )
			{
				return Error( "invalid hexadecimal digit spacing" );
			}
			
			n_digits_since_space = 0;
			
			continue;
		}
		
		++n_digits;
		++n_digits_since_space;
	}
	
	size_t n_data_bytes = n_digits / 2 + (n_digits & 1);
	size_t n_zero_bytes = mask - (n_data_bytes + mask & mask);
	size_t n_bytes      = n_data_bytes + n_zero_bytes;
	
	plus::string result;
	
	char* p = result.reset( n_bytes );
	
	while ( n_zero_bytes-- )
	{
		*p++ = 0;
	}
	
	size_t n = n_data_bytes;
	
	if ( const bool has_unpaired_digit = n_digits & 1 )
	{
		*p++ = gear::decoded_hex_digit( *data++ );
		
		--n;
	}
	
	for ( ;  n > 0;  --n )
	{
		unsigned char c = *data++;
		
		while ( c == ' ' )
		{
			c = *data++;
		}
		
		c  = gear::decoded_hex_digit( c       ) << 4;
		c |= gear::decoded_hex_digit( *data++ );
		
		*p++ = c;
	}
	
	return String( result );
}

}
