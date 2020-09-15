/*
	integer_hex.cc
	--------------
*/

#include "bignum/integer_hex.hh"

// iota
#include "iota/endian.hh"

// plus
#include "plus/hexadecimal.hh"


namespace bignum
{
	
	using plus::string;
	
	
	static inline
	iota::span straight_int_buffer( const integer& x )
	{
		return x.buffer();
	}
	
	static
	string reversed_int_buffer( const integer& x )
	{
		iota::span buffer = x.buffer();
		
		const char* p = buffer.data();
		unsigned long n_bytes = buffer.size();
		
		string result;
		
		char* begin = result.reset( n_bytes );
		
		char* q = begin + n_bytes;
		
		while ( n_bytes-- > 0 )
		{
			// Fill the result with the reversed buffer.
			*--q = *p++;
		}
		
		return result;
	}
	
	template < class Buffer >
	static
	string hex_from_int_buffer( const Buffer& buffer )
	{
		const char* p = buffer.data();
		const char* q = buffer.size() + p;
		
		// Omit leading zero bytes so the result is not dependent on limb size.
		
		while ( *p == 0 )
		{
			++p;
		}
		
		return plus::hex_lower( p, q - p );
	}
	
	string hex( const integer& x )
	{
		if ( x.is_zero() )
		{
			return string::null;
		}
		
		const bool le = iota::is_little_endian();
		
		return le ? hex_from_int_buffer( reversed_int_buffer( x ) )
		          : hex_from_int_buffer( straight_int_buffer( x ) );
	}
	
}
