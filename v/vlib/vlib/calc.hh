/*
	calc.hh
	-------
*/

#ifndef VLIB_CALC_HH
#define VLIB_CALC_HH

// vlib
#include "vlib/op_type.hh"
#include "vlib/value.hh"


namespace vlib
{
	
	Value calc( const Value&  left,
	            op_type       op,
	            const Value&  right );
	
}

#endif
