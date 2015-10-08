/*
	eval.hh
	-------
*/

#ifndef VC_EVAL_HH
#define VC_EVAL_HH

// vc
#include "vc/op_type.hh"
#include "vc/value.hh"


namespace vc
{
	
	Value eval( Value v );
	
	Value eval( Value    left,
	            op_type  op,
	            Value    right );
	
}

#endif
