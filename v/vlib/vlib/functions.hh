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
	
	void define( const char* name, function_type f );
	
	Value v_unbin( const Value& v );
	Value v_unhex( const Value& v );
	
	function_type function_from_name( const plus::string& name );
	
}

#endif
