/*	===============
 *	iota/decimal.hh
 *	===============
 */

#ifndef IOTA_DECIMAL_HH
#define IOTA_DECIMAL_HH


namespace iota
{
	
	inline unsigned pure_decimal_magnitude( unsigned x )
	{
		unsigned result = 0;
		
		while ( x > 0 )
		{
			x /= 10;
			
			++result;
		}
		
		return result;
	}
	
	inline unsigned decimal_magnitude( unsigned x )
	{
		return x == 0 ? 1 : pure_decimal_magnitude( x );
	}
	
	inline void fill_unsigned_decimal( unsigned x, char* begin, char* end )
	{
		for ( char* p = end;  p > begin;  x /= 10 )
		{
			*--p = '0' + x % 10;
		}
	}
	
	inline void fill_unsigned_decimal( unsigned x, char* begin, unsigned length )
	{
		fill_unsigned_decimal( x, begin, begin + length );
	}
	
	inline char* inscribe_unsigned_decimal_r( unsigned x, char* buffer )
	{
		const unsigned magnitude = decimal_magnitude( x );
		
		char* end = buffer + magnitude;
		
		fill_unsigned_decimal( x, buffer, end );
		
		return end;
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
	
}

#endif

