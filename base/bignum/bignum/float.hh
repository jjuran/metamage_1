/*
	float.hh
	--------
*/

#ifndef BIGNUM_FLOAT_HH
#define BIGNUM_FLOAT_HH

// bignum
#include "bignum/integer_fwd.hh"


namespace bignum
{
	
	float  to_float32( const bignum::integer& i );
	double to_float64( const bignum::integer& i );
	
	template < class Float >
	inline
	Float to_float( const bignum::integer& i );
	
	template <>
	inline
	float to_float< float >( const bignum::integer& i )
	{
		return to_float32( i );
	}
	
	template <>
	inline
	double to_float< double >( const bignum::integer& i )
	{
		return to_float64( i );
	}
	
}

#endif
