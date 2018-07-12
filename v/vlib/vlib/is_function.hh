/*
	is_function.hh
	--------------
*/

#ifndef VLIB_ISFUNCTION_HH
#define VLIB_ISFUNCTION_HH


namespace vlib
{
	
	class Value;
	
	bool is_function( const Value& v );
	
	bool is_functionally_impure( const Value& v );
	
}

#endif
