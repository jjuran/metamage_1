/*
	proc.cc
	-------
*/

#include "vlib/types/proc.hh"

// vlib
#include "vlib/dispatch/dispatch.hh"
#include "vlib/dispatch/verity.hh"
#include "vlib/types/builtin.hh"


namespace vlib
{
	
	static
	bool always_true( const Value& v )
	{
		return true;
	}
	
	static const veritization proc_veritization =
	{
		&always_true,
	};
	
	const dispatch proc_dispatch =
	{
		&builtin_stringifiers,
		&proc_veritization,
		&builtin_comparison,
	};
	
}
