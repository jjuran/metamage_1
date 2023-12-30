/*
	V.cc
	----
*/

#include "vlib/namespaces/V.hh"

// vlib
#include "vlib/namespace_info.hh"
#include "vlib/throw.hh"
#include "vlib/tracker.hh"
#include "vlib/value.hh"
#include "vlib/types/any.hh"
#include "vlib/types/namespace.hh"
#include "vlib/types/type.hh"


namespace vlib
{
	
	static
	Value getter( const plus::string& name )
	{
		if ( name == "one" )
		{
			return Type( one_vtype );
		}
		
		if ( name == "tracker" )
		{
			return Namespace( namespace_tracker );
		}
		
		THROW( "nonexistent member of namespace V" );
		
		return Value();
	}
	
	const namespace_info namespace_V =
	{
		"V",
		&getter,
	};
	
}
