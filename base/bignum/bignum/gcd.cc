/*
	gcd.cc
	------
*/

#include "bignum/gcd.hh"

// debug
#include "debug/assert.hh"


namespace bignum
{
	
	integer gcd( integer a, integer b )
	{
		ASSERT( ! a.is_zero() );
		ASSERT( ! b.is_zero() );
		
		if ( abs_compare( a, b ) > 0 )  // a > b
		{
			a.swap( b );
		}
		
		if ( b % a == 0 )
		{
			return abs( a );
		}
		
		integer result = 1;
		
		while ( a.is_even()  &&  b.is_even() )
		{
			result *= 2;
			
			a /= 2;
			b /= 2;
		}
		
		integer x = 3;
		
		while ( abs_compare( x, a ) <= 0 )  // x <= a
		{
			while ( a % x == 0  &&  b % x == 0 )
			{
				result *= x;
				
				a /= x;
				b /= x;
			}
			
			if ( abs_compare( x * x, a ) > 0 )  // x^2 > a
			{
				break;
			}
			
			x += 2;  // TODO:  Generate prime divisors only?
		}
		
		return result;
	}
	
}
