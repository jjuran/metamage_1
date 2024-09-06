/*
	float.cc
	--------
*/

#include "bignum/float.hh"

// iota
#include "iota/endian.hh"

// math
#include "math/integer.hh"

// bignum
#include "bignum/integer.hh"


#pragma exceptions off


namespace bignum
{

template < class Float >
static inline
Float _to_float( const bignum::integer& i )
{
	using math::integer::float_cast;
	
	const bool little = iota::is_little_endian();
	
	Float f = float_cast< Float >( little, i.data(), i.size() );
	
	if ( i.is_negative() )
	{
		f = -f;
	}
	
	return f;
}

float to_float32( const bignum::integer& i )
{
	return _to_float< float >( i );
}

double to_float64( const bignum::integer& i )
{
	return _to_float< double >( i );
}

}
