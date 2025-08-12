/*
	reference.cc
	------------
*/

#include "vlib/reference.hh"

// vlib
#include "vlib/targets.hh"
#include "vlib/throw.hh"


namespace vlib
{
	
	const Value& dereference( const Value& referent )
	{
		Target target = make_target( referent );
		
		if ( is_undefined( *target.addr ) )
		{
			THROW( "undefined value in dereference" );
		}
		
		return *target.addr;
	}
	
}
