/*
	library.hh
	----------
*/

#ifndef VLIB_LIBRARY_HH
#define VLIB_LIBRARY_HH

// vlib
#include "vlib/value.hh"


namespace vlib
{
	
	void define( const char* name, function_type f );
	
	Value v_getenv( const Value& );
	Value v_print ( const Value& );
	Value v_time  ( const Value& );
	
}

#endif
