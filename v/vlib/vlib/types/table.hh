/*
	table.hh
	--------
*/

#ifndef VLIB_TYPES_TABLE_HH
#define VLIB_TYPES_TABLE_HH

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
	
	class Table : public Value
	{
		public:
			static bool test( const Value& v )
			{
				return is_table( v );
			}
			
			Table( const Value& key_type, const Value& array );
	};
	
}

#endif
