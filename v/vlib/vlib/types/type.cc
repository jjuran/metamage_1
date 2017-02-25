/*
	type.cc
	-------
*/

#include "vlib/types/type.hh"

// vlib
#include "vlib/dispatch/dispatch.hh"
#include "vlib/dispatch/verity.hh"
#include "vlib/types/builtin.hh"
#include "vlib/types/null.hh"


namespace vlib
{
	
	static
	bool not_null( const Value& v )
	{
		return &v.typeinfo() != &null_vtype;
	}
	
	static const veritization type_veritization =
	{
		&not_null,
	};
	
	const dispatch type_dispatch =
	{
		&builtin_stringifiers,
		&type_veritization,
	};
	
}
