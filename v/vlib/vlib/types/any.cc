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
	
	bool is_etc( const Value& type )
	{
		if ( type.type() == Value_base_type )
		{
			const type_info& typeinfo = type.typeinfo();
			
			return &typeinfo == &etc_vtype;
		}
		
		return false;
	}
	
}
