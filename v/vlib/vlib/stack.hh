/*
	stack.hh
	--------
*/

#ifndef VLIB_STACK_HH
#define VLIB_STACK_HH

// vlib
#include "vlib/value.hh"


namespace vlib
{
	
	const Value& nth_frame( const Value& stack, unsigned depth );
	
}

#endif
