/*
	table-utils.hh
	--------------
*/

#ifndef VLIB_TABLEUTILS_HH
#define VLIB_TABLEUTILS_HH

// vlib
#include "vlib/value.hh"


namespace vlib
{
	
	Value associative_subscript( const Value& table, const Value& key );
	
	Value* get_table_subscript_addr( Expr* array_expr, const Value& key );
	
}

#endif
