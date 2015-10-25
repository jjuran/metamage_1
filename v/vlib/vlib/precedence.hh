/*
	precedence.hh
	-------------
*/

#ifndef VLIB_PRECEDENCE_HH
#define VLIB_PRECEDENCE_HH

// vlib
#include "vlib/op_type.hh"


namespace vlib
{
	
	bool decreasing_op_precedence( op_type left, op_type right );
	
}

#endif
