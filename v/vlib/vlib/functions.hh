/*
	functions.hh
	------------
*/

#ifndef VLIB_FUNCTIONS_HH
#define VLIB_FUNCTIONS_HH

// plus
#include "plus/string.hh"

// vlib
#include "vlib/value.hh"


namespace vlib
{
	
	function_type function_from_name( const plus::string& name );
	
}

#endif
