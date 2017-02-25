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
	
	inline
	bool is_table( const Value& v )
	{
		if ( Expr* expr = v.expr() )
		{
			if ( expr->op == Op_empower )
			{
				return is_type( expr->left )  &&  is_array( expr->right );
			}
		}
		
		return false;
	}
	
	Value make_table( const Value& key_type, const Value& array );
	
	Value associative_subscript( const Value& table, const Value& key );
	
	Value* get_table_subscript_addr( Expr* array_expr, const Value& key );
	
}

#endif
