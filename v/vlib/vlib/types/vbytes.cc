/*
	vbytes.cc
	---------
*/

#include "vlib/types/vbytes.hh"

// vlib
#include "vlib/dispatch/stringify.hh"


namespace vlib
{
	
	static
	const char* vbytes_cpy_data( const Value& v )
	{
		return v.string().data();
	}
	
	static
	size_t vbytes_cpy_size( const Value& v )
	{
		return v.string().size();
	}
	
	const stringify vbytes_cpy =
	{
		&vbytes_cpy_data,
		&vbytes_cpy_size,
		NULL,
	};
	
}
