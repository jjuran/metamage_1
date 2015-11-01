/*
	calc.hh
	-------
*/

#ifndef VLIB_CALC_HH
#define VLIB_CALC_HH

// vlib
#include "vlib/op_type.hh"
#include "vlib/proc_info.hh"
#include "vlib/value.hh"


namespace vlib
{
	
	extern const proc_info proc_join;
	
	Value calc( const Value&  left,
	            op_type       op,
	            const Value&  right );
	
}

#endif
