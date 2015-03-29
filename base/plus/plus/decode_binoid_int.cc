/*
	decode_binoid_int.cc
	--------------------
*/

#include "plus/decode_binoid_int.hh"

// iota
#include "iota/endian.hh"

// plus
#include "plus/binary.hh"
#include "plus/hexadecimal.hh"
#include "plus/var_string.hh"


namespace plus
{
	
	typedef integer::int_t  int_t;
	
	typedef string (*int_decoder)( const char*, unsigned long, align_t );
	
	
	static void reverse( char* begin, char* end )
	{
		char* p = begin;
		char* q = end;
		
		while ( p < q )
		{
			// Decrement q, swap *p and *q, increment p.
			const char c = *--q;
			*q = *p;
			*p++ = c;
		}
	}
	
	static void reverse_in_place( plus::string& s )
	{
		plus::var_string vs = s.move();
		
		reverse( vs.begin(), vs.end() );
		
		s = vs.move();
	}
	
	static
	integer decode_binoid_int( int_decoder f, const char* p, const char* q )
	{
		while ( p < q  &&  *p == '0' )
		{
			++p;
		}
		
		if ( p == q )
		{
			return integer();
		}
		
		typedef integer::size_type size_t;
		
		const size_t n_digits = q - p;
		
		const int align = sizeof (int_t);
		
		string s = f( p, n_digits, align );
		
		if ( iota::is_little_endian() )
		{
			reverse_in_place( s );
		}
		
		integer result( (int_t const*) s.data(),
		                s.size() / sizeof (int_t) );
		
		return result;
	}
	
	static inline
	integer decode_binoid_int( int_decoder f, const char* p, unsigned n )
	{
		return decode_binoid_int( f, p, p + n );
	}
	
	integer unbin_int( const char* p, unsigned n )
	{
		return decode_binoid_int( &unbin, p, n );
	}
	
	integer unhex_int( const char* p, unsigned n )
	{
		return decode_binoid_int( &unhex, p, n );
	}
	
}
