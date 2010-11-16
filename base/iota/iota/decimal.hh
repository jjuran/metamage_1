/*	===============
 *	iota/decimal.hh
 *	===============
 */

#ifndef IOTA_DECIMAL_HH
#define IOTA_DECIMAL_HH


namespace iota
{
	
	unsigned parse_unsigned_decimal( const char **pp );
	
	int parse_decimal( const char **pp );
	
	inline unsigned parse_unsigned_decimal( const char *p )
	{
		return parse_unsigned_decimal( &p );
	}
	
	inline int parse_decimal( const char *p )
	{
		return parse_decimal( &p );
	}
	
	
	template < unsigned base, class Type >
	unsigned pure_magnitude( Type x )
	{
		unsigned result = 0;
		
		while ( x > 0 )
		{
			x /= base;
			
			++result;
		}
		
		return result;
	}
	
	template < unsigned base, class Type >
	inline unsigned magnitude( Type x )
	{
		return x == 0 ? 1 : pure_magnitude< base >( x );
	}
	
	template < unsigned base, class Type >
	void fill_unsigned( Type x, char* begin, char* end )
	{
		for ( char* p = end;  p > begin;  x /= base )
		{
			*--p = '0' + x % base;
		}
	}
	
	template < unsigned base, class Type >
	inline void fill_unsigned( Type x, char* begin, unsigned length )
	{
		fill_unsigned< base >( x, begin, begin + length );
	}
	
	template < unsigned base, class Type >
	inline char* inscribe_unsigned_r( Type x, char* buffer )
	{
		const unsigned length = magnitude< base >( x );
		
		char* end = buffer + length;
		
		fill_unsigned< base >( x, buffer, end );
		
		return end;
	}
	
	inline unsigned pure_decimal_magnitude( unsigned x )
	{
		return pure_magnitude< 10 >( x );
	}
	
	inline unsigned decimal_magnitude( unsigned x )
	{
		return magnitude< 10 >( x );
	}
	
	inline void fill_unsigned_decimal( unsigned x, char* begin, char* end )
	{
		fill_unsigned< 10 >( x, begin, end );
	}
	
	inline void fill_unsigned_decimal( unsigned x, char* begin, unsigned length )
	{
		fill_unsigned< 10 >( x, begin, length );
	}
	
	inline char* inscribe_unsigned_decimal_r( unsigned x, char* buffer )
	{
		return inscribe_unsigned_r< 10 >( x, buffer );
	}
	
	inline char* inscribe_decimal_r( int x, char* buffer )
	{
		if ( x < 0 )
		{
			*buffer++ = '-';
			
			x = -x;
		}
		
		return inscribe_unsigned_decimal_r( x, buffer );
	}
	
	char* inscribe_unsigned_decimal( unsigned x );
	
	char* inscribe_decimal( int x );
	
	char* inscribe_unsigned_wide_decimal( unsigned long long x );
	
}

#endif

