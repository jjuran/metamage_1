/*
	pack_bits.cc
	------------
*/

#include "qd/pack_bits.hh"

// Standard C
#include <string.h>

// more-libc
#include "more/string.h"


namespace quickdraw
{
	
	static
	const uint8_t* find_end_of_run( const uint8_t* p, const uint8_t* end )
	{
		uint8_t c = *p++;
		
		while ( p < end )
		{
			if ( *p++ != c )
			{
				return --p;
			}
		}
		
		return p;
	}
	
	static
	const uint8_t* find_double( const uint8_t* p, const uint8_t* end )
	{
		uint8_t a = *p++;
		
		while ( p < end )
		{
			uint8_t b = *p++;
			
			if ( a == b )
			{
				return p - 2;
			}
			
			a = b;
		}
		
		return NULL;
	}
	
	static
	const uint8_t* find_triple( const uint8_t* p, const uint8_t* end )
	{
		while ( p < end )
		{
			p = find_double( p, end );
			
			if ( p == NULL  ||  p >= end - 2 )
			{
				return NULL;
			}
			
			if ( p[ 0 ] == p[ 2 ] )
			{
				return p;
			}
			
			p += 2;
		}
		
		return NULL;
	}
	
	void pack_bits( uint8_t const*& src, unsigned n_src, uint8_t*& dst )
	{
		uint8_t const* p = src;
		uint8_t*       r = dst;
		
		unsigned n = n_src;
		
		while ( n > 127 )
		{
			*r++ = 127;
			
			r = (uint8_t*) mempcpy( r, p, 128 );
			
			p += 128;
			n -= 128;
		}
		
		if ( n )
		{
			*r++ = n - 1;
			
			r = (uint8_t*) mempcpy( r, p, n );
			
			p += n;
		}
		
		src = p;
		dst = r;
	}
	
	static inline
	unsigned min( unsigned a, unsigned b )
	{
		return b < a ? b : a;
	}
	
	void unpack_bits( uint8_t const*& src, uint8_t*& dst, unsigned n_dst )
	{
		uint8_t const* p = src;
		uint8_t*       r = dst;
		
		unsigned N = n_dst;
		
		while ( N > 0 )
		{
			signed char c = *p++;
			
			if ( c >= 0 )
			{
				size_t n = min( N, 1 + c );
				
				r = (uint8_t*) mempcpy( r, p, n );
				
				p += n;
				N -= n;
			}
			else if ( c == (signed char) 0x80 )
			{
				/*
					The behavior of UnpackBits() in the presence of this
					metadata byte is undocumented, and varies across versions
					of Mac OS.  We can't securely process the input without
					knowing which behavior the caller is expecting.
				*/
				--p;
				break;
			}
			else
			{
				size_t n = min( N, 1 - c );
				
				memset( r, *p++, n );
				
				r += n;
				N -= n;
			}
		}
		
		src = p;
		dst = r;
	}
	
}
