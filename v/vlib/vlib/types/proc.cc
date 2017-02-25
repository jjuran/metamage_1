/*
	proc.cc
	-------
*/

#include "vlib/types/proc.hh"

// vlib
#include "vlib/dispatch/dispatch.hh"
#include "vlib/types/builtin.hh"


namespace vlib
{
	
	const dispatch proc_dispatch =
	{
		&builtin_stringifiers,
	};
	
}
