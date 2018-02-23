/*
	Math.cc
	-------
*/

#include "vlib/namespaces/Math.hh"

// vlib
#include "vlib/functions.hh"
#include "vlib/namespace_info.hh"
#include "vlib/throw.hh"
#include "vlib/value.hh"
#include "vlib/types/namespace.hh"
#include "vlib/types/proc.hh"
#include "vlib/types/string.hh"


namespace vlib
{
	
	static
	Value getter( const plus::string& name )
	{
		if ( name == "name" )
		{
			return String( "Math" );
		}
		
		if ( name == "abs" )
		{
			return Proc( proc_abs );
		}
		
		if ( name == "half" )
		{
			return Proc( proc_half );
		}
		
		THROW( "nonexistent member of namespace Math" );
		
		return Value();
	}
	
	const namespace_info namespace_Math =
	{
		"Math",
		&getter,
	};
	
}
