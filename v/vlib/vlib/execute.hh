/*
	execute.hh
	----------
*/

#ifndef VLIB_EXECUTE_HH
#define VLIB_EXECUTE_HH

// vlib
#include "vlib/value.hh"


namespace vlib
{
	
	inline
	bool is_type_annotation( const Value& v )
	{
		if ( Expr* expr = v.expr() )
		{
			return expr->op == Op_denote;
		}
		
		return false;
	}
	
	Value execute( const Value& root );
	
}

#endif
