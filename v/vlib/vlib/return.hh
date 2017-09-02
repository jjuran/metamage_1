/*
	return.hh
	---------
*/

#ifndef VLIB_RETURN_HH
#define VLIB_RETURN_HH

// vlib
#include "vlib/source.hh"
#include "vlib/value.hh"


namespace vlib
{
	
	struct transfer_via_return
	{
		const Value        object;
		const source_spec  source;
		
		transfer_via_return( const Value& obj, const source_spec& src )
		:
			object( obj ),
			source( src )
		{
		}
	};
	
}

#endif
