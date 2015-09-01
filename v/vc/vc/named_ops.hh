/*
	named_ops.hh
	------------
*/

#ifndef VC_NAMEDOPS_HH
#define VC_NAMEDOPS_HH

// plus
#include "plus/string.hh"

// vc
#include "vc/op_type.hh"


namespace vc
{
	
	op_type op_from_name( const plus::string& name );
	
}

#endif
