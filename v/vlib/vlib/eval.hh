/*
	eval.hh
	-------
*/

#ifndef VLIB_EVAL_HH
#define VLIB_EVAL_HH

// vlib
#include "vlib/op_type.hh"
#include "vlib/value.hh"


namespace vlib
{
	
	Value eval( Value v );
	
	Value eval( Value    left,
	            op_type  op,
	            Value    right );
	
}

#endif
