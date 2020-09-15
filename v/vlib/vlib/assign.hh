/*
	assign.hh
	---------
*/

#ifndef VLIB_ASSIGN_HH
#define VLIB_ASSIGN_HH

// vlib
#include "vlib/value.hh"
#include "vlib/target.hh"


namespace vlib
{
	
	Value as_assigned( const Value& type, const Value& v );
	
	void assign( const Target&  target,
	             const Value&   v,
	             bool           coercive = false );
	
}

#endif
