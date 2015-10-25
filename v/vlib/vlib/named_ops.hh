/*
	named_ops.hh
	------------
*/

#ifndef VLIB_NAMEDOPS_HH
#define VLIB_NAMEDOPS_HH

// plus
#include "plus/string.hh"

// vlib
#include "vlib/op_type.hh"


namespace vlib
{
	
	op_type op_from_name( const plus::string& name );
	
}

#endif
