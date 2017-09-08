/*
	V.cc
	----
*/

#include "vlib/namespaces/V.hh"

// vlib
#include "vlib/namespace_info.hh"
#include "vlib/throw.hh"
#include "vlib/value.hh"
#include "vlib/types/string.hh"


namespace vlib
{
	
	static
	Value getter( const plus::string& name )
	{
		if ( name == "name" )
		{
			return String( "V" );
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
