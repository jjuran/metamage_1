/*
	compare.hh
	----------
*/

#ifndef VLIB_DISPATCH_COMPARE_HH
#define VLIB_DISPATCH_COMPARE_HH

// math
#include "math/integer_types.hh"


namespace vlib
{
	
	using math::integer::cmp_t;
	
	class Value;
	
	typedef cmp_t (*ordering)( const Value& a, const Value& b );
	typedef bool  (*equality)( const Value& a, const Value& b );
	
	struct comparison
	{
		ordering  order;
		equality  equal;
	};
	
}

#endif
