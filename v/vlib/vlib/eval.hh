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
	
	const Value& eval( const Value& v );
	
	Value eval( const Value&  left,
	            op_type       op,
	            const Value&  right );
	
}

#endif
