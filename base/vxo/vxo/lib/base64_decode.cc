/*
	base64_decode.cc
	----------------
*/

#include "vxo/lib/base64_decode.hh"

// iota
#include "iota/char_types.hh"

// plus
#include "plus/string.hh"


#pragma exceptions off


/*
	Base64 code:
		
		ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=
*/

namespace vxo
{

static inline
int8_t decode( char c )
{
	return + iota::is_upper( c ) ? c - 'A'
	       : iota::is_lower( c ) ? c - 'a' + 26
	       : iota::is_digit( c ) ? c - '0' + 52
	       : c == '+'            ? 62
	       : c == '/'            ? 63
	       :                       -1;
}

Expected< String > base64_decode( const char* data, size_t padded_size )
{
	if ( padded_size & 0x03 )
	{
		return Error( "base64 size not a multiple of four bytes" );
	}
	
	size_t z = padded_size;
	
	int pad = 0;
	
	if ( ! z  ||  (data[ --z ] == '=' && (++pad, data[ --z ] == '=') && ++pad) )
	{
		// If we get here, we've correctly decremented z twice or not at all.
	}
	else
	{
		++z;  // compensate for overdecrement
	}
	
	for ( size_t i = 0;  i < z;  ++i )
	{
		if ( decode( data[ i ] ) < 0 )
		{
			return Error( "invalid base64 digit" );
		}
	}
	
	size_t n_bytes = padded_size / 4 * 3 - pad;
	
	size_t n_full_units = z / 4;
	
	plus::string result;
	
	char* p = result.reset_nothrow( n_bytes );
	
	if ( p == NULL )
	{
		return Error( out_of_memory );
	}
	
	for ( size_t i = 0;  i < n_full_units;  ++i )
	{
		uint32_t x = decode( *data++ );
		
		x <<= 6;  x |= decode( *data++ );
		x <<= 6;  x |= decode( *data++ );
		x <<= 6;  x |= decode( *data++ );
		
		uint8_t c = x;  x >>= 8;
		uint8_t b = x;  x >>= 8;
		uint8_t a = x;
		
		*p++ = a;
		*p++ = b;
		*p++ = c;
	}
	
	if ( pad-- )
	{
		uint32_t x = decode( *data++ );
		
		x <<= 6;  x |= decode( *data++ );
		
		*p++ = x >> 4;
		
		x &= ~0x0ff0;
		
		if ( ! pad )
		{
			x <<= 6;  x |= decode( *data++ );
			
			*p++ = x >> 2;
			
			x &= ~0x03fc;
		}
		
		if ( x != 0 )
		{
			return Error( "invalid non-zero base64 padding" );
		}
	}
	
	return String( result );
}

}
