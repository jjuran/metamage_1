/*
	pure.hh
	-------
*/

#ifndef VC_PURE_HH
#define VC_PURE_HH

// vc
#include "vc/op_type.hh"
#include "vc/value.hh"


namespace vc
{
	
	Value eval_pure( const Value&  left,
	                 op_type       op,
	                 const Value&  right );
	
}

#endif
