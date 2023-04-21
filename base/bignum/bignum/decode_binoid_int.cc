/*
	decode_binoid_int.cc
	--------------------
*/

#include "bignum/decode_binoid_int.hh"

// iota
#include "iota/endian.hh"

// vxo-string
#include "vxo-string/lib/unbin.hh"
#include "vxo-string/lib/unhex.hh"

// plus
#include "plus/reverse.hh"
#include "plus/string.hh"


namespace bignum
{
	
	using plus::string;
	
	typedef integer::int_t  int_t;
	
	typedef vxo::Box (*int_decoder)( const char*, unsigned long, vxo::align_t );
	
	
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
		
		/*
			The VXO decoders can return errors, but we assume
			that the input has been validated via tokenization.
		*/
		
		vxo::Box box = f( p, n_digits, align );
		
		string& s = *(string*) &box;
		
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
		return decode_binoid_int( &vxo::unbin, p, n );
	}
	
	integer unhex_int( const char* p, unsigned n )
	{
		return decode_binoid_int( &vxo::unhex, p, n );
	}
	
}
