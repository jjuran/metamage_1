/*
	named_ops.hh
	------------
*/

#ifndef VC_NAMEDOPS_HH
#define VC_NAMEDOPS_HH

// plus
#include "plus/string_fwd.hh"

// vc
#include "vc/op_type.hh"
#include "vc/token.hh"


namespace vc
{
	
	op_type op_from_name( const plus::string& name );
	
}

#endif
