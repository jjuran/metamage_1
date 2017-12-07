/*
	is_type.cc
	----------
*/

#include "vlib/is_type.hh"

// vlib
#include "vlib/value.hh"


namespace vlib
{
	
	bool is_type( const Value& v )
	{
		if ( Expr* expr = v.expr() )
		{
			if ( expr->op == Op_subscript )
			{
				if ( is_type( expr->left ) )
				{
					// subscript type
					return is_empty_list( expr->right );
				}
			}
			
			if ( expr->op == Op_unary_deref )
			{
				return is_type( expr->right );
			}
			
			if ( expr->op == Op_empower )
			{
				return is_type( expr->left )  &&  is_type( expr->right );
			}
			
			if ( expr->op == Op_mapping )
			{
				return is_type( expr->left )  &&  is_type( expr->right );
			}
			
			if ( expr->op == Op_intersection  ||  expr->op == Op_union )
			{
				return is_type( expr->left )  &&  is_type( expr->right );
			}
		}
		
		return v.type() == Value_base_type  ||  is_empty_list( v );
	}
	
}
