/*
	fraction.hh
	-----------
*/

#ifndef BIGNUM_FRACTION_HH
#define BIGNUM_FRACTION_HH

// bignum
#include "bignum/integer.hh"


namespace bignum
{
	
	class fraction
	{
		public:
			typedef math::integer::cmp_t  cmp_t;
		
		private:
			integer its_numer;
			integer its_denom;
			
			void reduce();
			
			friend void equalize_denominators( fraction& a, fraction& b );
		
		public:
			template < class T >
			static fraction make( T numer )
			{
				return fraction( integer( numer ) );
			}
			
			fraction() : its_denom( 1 )
			{
			}
			
			fraction( const integer& numer )
			:
				its_numer( numer ),
				its_denom( 1 )
			{
			}
			
			fraction( const integer& numer, const integer& denom );
			
			const integer& numerator()   const  { return its_numer; }
			const integer& denominator() const  { return its_denom; }
			
			const integer& numer() const  { return its_numer; }
			const integer& denom() const  { return its_denom; }
			
			void reciprocate()  { its_numer.swap( its_denom ); }
			
			void swap( fraction& other )
			{
				its_numer.swap( other.its_numer );
				its_denom.swap( other.its_denom );
			}
			
			bool is_zero    () const  { return its_numer.is_zero();     }
			bool is_positive() const  { return its_numer.is_positive(); }
			bool is_negative() const  { return its_numer.is_negative(); }
			
			ibox::sign_t sign() const  { return its_numer.sign(); }
			
			void invert()  { its_numer.invert(); }
			
			void absolve()  { if ( is_negative() )  invert(); }
			
			friend unsigned long area( const fraction& f )
			{
				return area( f.its_numer ) + area( f.its_denom );
			}
			
			friend cmp_t abs_compare( fraction a, fraction b );
			
			friend cmp_t compare( fraction a, fraction b );
			
			fraction& operator+=( const fraction& y );
			fraction& operator-=( const fraction& y );
			fraction& operator*=( const fraction& y );
			fraction& operator/=( const fraction& y );
	};
	
	
	inline
	void swap( fraction& a, fraction& b )
	{
		a.swap( b );
	}
	
	inline
	bool operator==( const fraction& a, const fraction& b )
	{
		return compare( a, b ) == 0;
	}
	
	inline
	bool operator!=( const fraction& a, const fraction& b )
	{
		return compare( a, b ) != 0;
	}
	
	inline
	bool operator<( const fraction& a, const fraction& b )
	{
		return compare( a, b ) < 0;
	}
	
	inline
	bool operator>( const fraction& a, const fraction& b )
	{
		return compare( a, b ) > 0;
	}
	
	inline
	bool operator<=( const fraction& a, const fraction& b )
	{
		return compare( a, b ) <= 0;
	}
	
	inline
	bool operator>=( const fraction& a, const fraction& b )
	{
		return compare( a, b ) >= 0;
	}
	
	inline
	const fraction& operator+( const fraction& a )
	{
		return a;
	}
	
	inline
	fraction abs( fraction x )
	{
		x.absolve();
		
		return x;
	}
	
	inline
	fraction reciprocal( fraction x )
	{
		x.reciprocate();
		
		return x;
	}
	
	inline
	fraction operator-( fraction a )
	{
		a.invert();
		
		return a;
	}
	
	inline
	fraction operator+( fraction a, const fraction& b )
	{
		return a += b;
	}
	
	inline
	fraction operator-( fraction a, const fraction& b )
	{
		return a -= b;
	}
	
	inline
	fraction operator*( fraction a, const fraction& b )
	{
		return a *= b;
	}
	
	inline
	fraction operator/( fraction a, const fraction& b )
	{
		return a /= b;
	}
	
	fraction raise_to_power( fraction base, integer exponent );
	
}

#endif
