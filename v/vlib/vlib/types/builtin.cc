/*
	builtin.cc
	----------
*/

#include "vlib/types/builtin.hh"

// vlib
#include "vlib/value.hh"
#include "vlib/dispatch/stringify.hh"


namespace vlib
{
	
	/*
		proc_info and type_info can be thought of as extending builtin_info,
		although they don't actually derive from it.
	*/
	
	struct builtin_info
	{
		char const* const  name;
	};
	
	static
	const char* builtin_str_data( const Value& v )
	{
		return v.dereference< builtin_info >().name;
	}
	
	static const stringify builtin_str =
	{
		&builtin_str_data,
		NULL,
		NULL,
	};
	
	const stringifiers builtin_stringifiers =
	{
		&builtin_str,
		// rep: ditto
		// bin: not defined
	};
	
}
