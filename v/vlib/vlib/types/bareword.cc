/*
	bareword.cc
	-----------
*/

#include "vlib/types/bareword.hh"

// vlib
#include "vlib/dispatch/dispatch.hh"
#include "vlib/dispatch/stringify.hh"
#include "vlib/types/vbytes.hh"


namespace vlib
{
	
	static const stringifiers string_stringifiers =
	{
		&vbytes_cpy,
		// ditto for rep
		// bin not implemented
	};
	
	const dispatch bareword_dispatch =
	{
		&string_stringifiers,
		NULL,
		NULL,
		NULL,
	};
	
}
