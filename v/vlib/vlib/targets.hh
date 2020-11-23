/*
	targets.hh
	----------
*/

#ifndef VLIB_TARGETS_HH
#define VLIB_TARGETS_HH

// vlib
#include "vlib/target.hh"
#include "vlib/value.hh"


namespace vlib
{
	
	Target make_target( const Value& dst, bool spoiling = false );
	
}

#endif
