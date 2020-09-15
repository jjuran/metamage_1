/*
	decimal.cc
	----------
*/

#include "bignum/decimal.hh"

// bignum
#include "bignum/nth_power_of_ten.hh"


namespace bignum
{
	
	using plus::string;
	
	
	static inline
	bool abs_lte( const integer& a, const integer& b )
	{
		return abs_compare( a, b ) <= 0;
	}
	
	static inline
	bool abs_gte( const integer& a, const integer& b )
	{
		return abs_compare( a, b ) >= 0;
	}
	
	
	integer decode_decimal( const char* p, unsigned n )
	{
		integer result;
		
		bool negative = false;
		
		if ( n != 0 )
		{
			switch ( *p )
			{
				case '-':
					negative = true;
					// fall through
				
				case '+':
					++p;
					--n;
					break;
				
				default:
					break;
			}
		}
		
		while ( n-- )
		{
			if ( char c = *p++ - '0' )
			{
				integer digit = c;
			
				result += nth_power_of_ten( n ) * digit;
			}
		}
		
		if ( negative )
		{
			result.invert();
		}
		
		return result;
	}
	
	static string::size_type count_decimal_digits( const integer& x )
	{
		string::size_type power = 0;
		
		while ( abs_lte( nth_power_of_ten( power ), x ) )
		{
			++power;
		}
		
		return power;
	}
	
	string::size_type decimal_length( const integer& x )
	{
		return count_decimal_digits( x ) + ! x.is_positive();
	}
	
	
	static
	char* encode_decimal( char* r, string::size_type n, const integer& x )
	{
		if ( x.is_zero() )
		{
			*r++ = '0';
			
			return r;
		}
		
		integer remains = x;
		
		const bool negative = x.is_negative();
		
		if ( negative )
		{
			remains.invert();
		}
		
		if ( negative )
		{
			*r++ = '-';
		}
		
		while ( n-- > 0 )
		{
			const integer& x10 = nth_power_of_ten( n );
			
			int i = 0;
			
			while ( abs_gte( remains, x10 ) )
			{
				remains -= x10;
				++i;
			}
			
			*r++ = '0' + i;
		}
		
		return r;
	}
	
	char* encode_decimal( char* r, const integer& x )
	{
		string::size_type n = count_decimal_digits( x );
		
		return encode_decimal( r, n, x );
	}
	
	string encode_decimal( const integer& x )
	{
		const bool negative = x.is_negative();
		
		string::size_type n = count_decimal_digits( x );
		
		string::size_type size = n + ! x.is_positive();  // "-" or "0"
		
		plus::string result;
		
		char* r = result.reset( size );
		
		encode_decimal( r, n, x );
		
		return result;
	}
	
}
