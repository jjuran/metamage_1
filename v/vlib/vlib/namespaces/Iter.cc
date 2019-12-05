/*
	Iter.cc
	-------
*/

#include "vlib/namespaces/Iter.hh"

// vlib
#include "vlib/namespace_info.hh"
#include "vlib/order.hh"
#include "vlib/throw.hh"
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
			return String( "Iter" );
		}
		
		if ( name == "max" )
		{
			return Proc( proc_It_max );
		}
		
		if ( name == "min" )
		{
			return Proc( proc_It_min );
		}
		
		THROW( "nonexistent member of namespace Iter" );
		
		return Value();
	}
	
	const namespace_info namespace_Iter =
	{
		"Iter",
		&getter,
	};
	
}
