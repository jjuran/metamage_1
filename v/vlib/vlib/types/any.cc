/*
	any.cc
	------
*/

#include "vlib/types/any.hh"

// vlib
#include "vlib/type_info.hh"
#include "vlib/value.hh"


namespace vlib
{
	
	static
	Value identity( const Value& v )
	{
		return v;
	}
	
	const type_info etc_vtype = { "...", &identity, 0, 0 };
	
}
