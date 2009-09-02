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
	
	inline char* inscribe_unsigned_decimal_backwards( unsigned x, char* end )
	{
		char* p = end;
		
		if ( x == 0 )
		{
			*--p = '0';
			
			return p;
		}
		
		while ( x != 0 )
		{
			*--p = '0' + x % 10;
			
			x /= 10;
		}
		
		return p;
	}
	
	inline char* inscribe_unsigned_decimal_r( unsigned x, char* buffer )
	{
		const unsigned magnitude = decimal_magnitude( x );
		
		char* end = buffer + magnitude;
		
		inscribe_unsigned_decimal_backwards( x, end );
		
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

