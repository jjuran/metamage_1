/*
	ops.hh
	------
*/

#ifndef VLIB_OPS_HH
#define VLIB_OPS_HH

// vlib
#include "vlib/op_type.hh"
#include "vlib/token_type.hh"


namespace vlib
{
	
	op_type op_from_token( token_type token, bool expecting_value );
	
}

#endif
