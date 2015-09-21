/*
	eval.hh
	-------
*/

#ifndef VC_EVAL_HH
#define VC_EVAL_HH

// plus
#include "plus/integer.hh"

// vc
#include "vc/op_type.hh"
#include "vc/value.hh"


namespace vc
{
	
	Value eval( const Value&  left,
	            op_type       op,
	            const Value&  right );
	
}

#endif
