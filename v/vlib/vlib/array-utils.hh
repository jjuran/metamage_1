/*
	array-utils.hh
	--------------
*/

#ifndef VLIB_ARRAYUTILS_HH
#define VLIB_ARRAYUTILS_HH

// vlib
#include "vlib/value.hh"


namespace vlib
{
	
	Value linear_subscript( const Value& array, const Value& index );
	
}

#endif
