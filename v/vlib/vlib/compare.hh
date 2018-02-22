/*
	compare.hh
	----------
*/

#ifndef VLIB_COMPARE_HH
#define VLIB_COMPARE_HH

// math
#include "math/integer.hh"


namespace vlib
{
	
	class Value;
	
	math::integer::cmp_t compare( const Value& a, const Value& b );
	
}

#endif
