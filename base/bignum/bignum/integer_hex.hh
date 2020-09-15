/*
	integer_hex.hh
	--------------
*/

#ifndef BIGNUM_INTEGERHEX_HH
#define BIGNUM_INTEGERHEX_HH

// plus
#include "plus/string.hh"

// bignum
#include "bignum/integer.hh"


namespace bignum
{
	
	plus::string hex( const integer& x );
	
}

#endif
