/*
	return.hh
	---------
*/

#ifndef VLIB_RETURN_HH
#define VLIB_RETURN_HH

// iota
#include "iota/class.hh"

// vlib
#include "vlib/in-flight.hh"
#include "vlib/source.hh"


namespace vlib
{
	
	struct transfer_via_return
	{
		const Value_in_flight  object;
		const source_spec      source;
		
		transfer_via_return( const Value& obj, const source_spec& src )
		:
			object( obj ),
			source( src )
		{
		}
		
		NO_NEW_DELETE
	};
	
}

#endif
