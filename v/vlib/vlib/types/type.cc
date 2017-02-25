/*
	type.cc
	-------
*/

#include "vlib/types/type.hh"

// vlib
#include "vlib/dispatch/dispatch.hh"
#include "vlib/types/builtin.hh"


namespace vlib
{
	
	const dispatch type_dispatch =
	{
		&builtin_stringifiers,
	};
	
}
