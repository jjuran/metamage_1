/*
	fraction.cc
	-----------
*/

#include "bignum/fraction.hh"

// debug
#include "debug/assert.hh"

// bignum
#include "bignum/gcd.hh"


namespace bignum
{
	
	using math::integer::cmp_t;
	
	
	void fraction::reduce()
	{
		if ( its_numer.is_zero() )
		{
			its_denom = 1;
		}
		else if ( its_denom != 1 )
		{
			const integer x = gcd( its_numer, its_denom );
			
			its_numer /= x;
			its_denom /= x;
		}
	}
	
	fraction::fraction( const integer& numer, const integer& denom )
	:
		its_numer( numer ),
		its_denom( denom )
	{
		ASSERT( denom != 0 );
		
		if ( denom.is_negative() )
		{
			its_numer.invert();
			its_denom.invert();
		}
		
		reduce();
	}
	
	void equalize_denominators( fraction& a, fraction& b )
	{
		if ( a.its_denom == b.its_denom )
		{
			return;
		}
		
		const integer x = gcd( a.its_denom, b.its_denom );
		
		const integer ax = b.its_denom / x;
		const integer bx = a.its_denom / x;
		
		a.its_numer *= ax;
		a.its_denom *= ax;
		b.its_numer *= bx;
		b.its_denom *= bx;
	}
	
	cmp_t abs_compare( fraction a, fraction b )
	{
		equalize_denominators( a, b );
		
		return abs_compare( a.its_numer, b.its_numer );
	}
	
	cmp_t compare( fraction a, fraction b )
	{
		equalize_denominators( a, b );
		
		return compare( a.its_numer, b.its_numer );
	}
	
	fraction& fraction::operator+=( const fraction& y )
	{
		fraction& x = *this;
		fraction& a = *this;
		
		if ( y.is_zero() )  return x;
		if ( x.is_zero() )  return x = y;
		
		fraction b = y;
		
		equalize_denominators( a, b );
		
		a.its_numer += b.its_numer;
		
		reduce();
		
		return a;
	}
	
	fraction& fraction::operator-=( const fraction& y )
	{
		return *this += -y;
	}
	
	fraction& fraction::operator*=( const fraction& y )
	{
		fraction& x = *this;
		
		if ( x.is_zero() )  return x;
		if ( y.is_zero() )  return x = y;
		
		x.its_numer *= y.its_numer;
		x.its_denom *= y.its_denom;
		
		reduce();
		
		return x;
	}
	
	fraction& fraction::operator/=( const fraction& y )
	{
		fraction& x = *this;
		
		if ( y.is_zero() )  throw division_by_zero();
		if ( x.is_zero() )  return x;
		
		x.its_numer *= y.its_denom;
		x.its_denom *= y.its_numer;
		
		reduce();
		
		return x;
	}
	
	fraction raise_to_power( fraction base, integer exponent )
	{
		const bool negative_exponent = exponent.is_negative();
		
		fraction result = fraction::make( 1 );
		
		while ( ! exponent.is_zero() )
		{
			if ( exponent.is_odd() )
			{
				result *= base;
			}
			
			base *= base;
			
			exponent.halve();
		}
		
		if ( negative_exponent )
		{
			result.reciprocate();
		}
		
		return result;
	}
	
}
