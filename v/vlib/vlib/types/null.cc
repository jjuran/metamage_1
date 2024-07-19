/*
	null.cc
	-------
*/

#include "vlib/types/null.hh"

// vlib
#include "vlib/type_info.hh"


namespace vlib
{
	
	Value Null::coerce( const Value& v )
	{
		return Null();
	}
	
	const type_info null_vtype =
	{
		"null",
		&assign_to< Null >,
		&Null::coerce,
		0,
		0,
		Type_pure,
	};
	
}
