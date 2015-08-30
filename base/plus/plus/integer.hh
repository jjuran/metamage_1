/*
	integer.hh
	----------
*/

#ifndef PLUS_INTEGER_HH
#define PLUS_INTEGER_HH

// math
#include "math/integer_types.hh"

// plus
#include "plus/ibox.hh"


namespace plus
{
	
	struct division_by_zero
	{
	};
	
	struct negative_exponent
	{
	};
	
	class integer
	{
		public:
			typedef math::integer::cmp_t  cmp_t;
			
			typedef ibox::int_t   int_t;
			typedef ibox::size_t  size_type;
		
		private:
			ibox box;
		
		public:
			integer() : box()
			{
			}
			
			integer( int_t const* data, size_type n )
			:
				box( data, n )
			{
			}
			
			integer(          int       i ) : box( (         long) i )  {}
			integer( unsigned int       i ) : box( (unsigned long) i )  {}
			integer(          long      i ) : box( i )  {}
			integer( unsigned long      i ) : box( i )  {}
			integer(          long long i ) : box( i )  {}
			integer( unsigned long long i ) : box( i )  {}
			
			void swap( integer& other )  { box.swap( other.box ); }
			
			size_type size() const  { return box.size(); }
			
			unsigned long byte_size() const  { return size() * sizeof (int_t); }
			
			void copy_into( char* buffer ) const;
			
			bool is_zero() const  { return box.sign() == 0; }
			bool is_positive() const  { return box.sign() > 0; }
			bool is_negative() const  { return box.sign() < 0; }
			
			bool is_odd()  const  { return box.odd();  }
			bool is_even() const  { return box.even(); }
			
			ibox::sign_t sign() const  { return box.sign(); }
			
			void halve()   { box.halve();  }
			void invert()  { box.invert(); }
			
			void absolve()  { if ( is_negative() )  invert(); }
			
			friend cmp_t abs_compare( const integer& a, const integer& b )
			{
				return abs_compare( a.box, b.box );
			}
			
			friend cmp_t compare( const integer& a, const integer& b )
			{
				return compare( a.box, b.box );
			}
			
			integer& operator++()  { return *this +=  1; }
			integer& operator--()  { return *this += -1; }
			
			integer operator++(int)  { integer x = *this; ++*this; return x; }
			integer operator--(int)  { integer x = *this; --*this; return x; }
			
			integer& operator+=( const integer& y );
			integer& operator-=( const integer& y );
			integer& operator*=( const integer& y );
			integer& operator/=( const integer& y );
			integer& operator%=( const integer& y );
			
			integer& modulo_by( const integer& modulus );
	};
	
	
	inline
	void swap( integer& a, integer& b )
	{
		a.swap( b );
	}
	
	inline
	integer half( integer x )
	{
		x.halve();
		return x;
	}
	
	inline
	bool operator==( const integer& a, const integer& b )
	{
		return compare( a, b ) == 0;
	}
	
	inline
	bool operator!=( const integer& a, const integer& b )
	{
		return compare( a, b ) != 0;
	}
	
	inline
	bool operator<( const integer& a, const integer& b )
	{
		return compare( a, b ) < 0;
	}
	
	inline
	bool operator>( const integer& a, const integer& b )
	{
		return compare( a, b ) > 0;
	}
	
	inline
	bool operator<=( const integer& a, const integer& b )
	{
		return compare( a, b ) <= 0;
	}
	
	inline
	bool operator>=( const integer& a, const integer& b )
	{
		return compare( a, b ) >= 0;
	}
	
	inline
	const integer& operator+( const integer& a )
	{
		return a;
	}
	
	inline
	integer abs( integer x )
	{
		x.absolve();
		
		return x;
	}
	
	inline
	integer operator-( integer a )
	{
		a.invert();
		
		return a;
	}
	
	inline
	integer operator+( integer a, const integer& b )
	{
		return a += b;
	}
	
	inline
	integer operator-( integer a, const integer& b )
	{
		return a -= b;
	}
	
	inline
	integer operator*( integer a, const integer& b )
	{
		return a *= b;
	}
	
	inline
	integer operator/( integer a, const integer& b )
	{
		return a /= b;
	}
	
	inline
	integer operator%( integer a, const integer& b )
	{
		return a %= b;
	}
	
	inline
	integer modulo( integer a, const integer& b )
	{
		return a.modulo_by( b );
	}
	
	integer raise_to_power( integer base, integer exponent );
	
}

#endif
