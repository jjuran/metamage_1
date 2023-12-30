/*
	Math.cc
	-------
*/

#include "vlib/namespaces/Math.hh"

// vlib
#include "vlib/functions.hh"
#include "vlib/namespace_info.hh"
#include "vlib/reduce.hh"
#include "vlib/throw.hh"
#include "vlib/value.hh"
#include "vlib/types/namespace.hh"
#include "vlib/types/proc.hh"


namespace vlib
{
	
	static
	Value getter( const plus::string& name )
	{
		if ( name == "half" )
		{
			return Proc( proc_half );
		}
		
		if ( name == "max" )
		{
			return Proc( proc_max );
		}
		
		if ( name == "min" )
		{
			return Proc( proc_min );
		}
		
		if ( name == "product" )
		{
			return Proc( proc_product );
		}
		
		if ( name == "sum" )
		{
			return Proc( proc_sum );
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
