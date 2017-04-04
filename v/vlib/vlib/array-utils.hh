/*
	array-utils.hh
	--------------
*/

#ifndef VLIB_ARRAYUTILS_HH
#define VLIB_ARRAYUTILS_HH

// vlib
#include "vlib/value.hh"


namespace vlib
{
	
	struct Target;
	
	unsigned subscript_integer( const Value& index );
	
	Value linear_subscript( const Value& array, const Value& index );
	
	void get_array_index_type( const Value& array_type, const Value*& base_type );
	
	Value* get_array_subscript_addr( Expr* array_expr, const Value& index );
	
	void push( const Target& array_target, const Value& list );
	
}

#endif
