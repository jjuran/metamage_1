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


namespace vc
{
	
	plus::integer eval( const plus::integer&  left,
	                    op_type               op,
	                    const plus::integer&  right );
	
}

#endif
