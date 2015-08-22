/*
	precedence.hh
	-------------
*/

#ifndef VC_PRECEDENCE_HH
#define VC_PRECEDENCE_HH

// vc
#include "vc/op_type.hh"


namespace vc
{
	
	bool decreasing_op_precedence( op_type left, op_type right );
	
}

#endif
