/*
	ops.hh
	------
*/

#ifndef VC_OPS_HH
#define VC_OPS_HH

// vc
#include "vc/op_type.hh"
#include "vc/token_type.hh"


namespace vc
{
	
	op_type op_from_token( token_type token, bool expecting_value );
	
}

#endif
