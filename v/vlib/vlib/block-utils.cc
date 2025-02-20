/*
	block-utils.cc
	--------------
*/

#include "vlib/block-utils.hh"

// vlib
#include "vlib/value.hh"


namespace vlib
{

const Value& unembrace( const Value& block )
{
	if ( Expr* expr = block.expr() )
	{
		if ( (op_type) (uint8_t) expr->op == Op_block )
		{
			if ( expr->right.is_evaluated() )
			{
				return expr->right;
			}
			
			expr = expr->right.expr();
			
			if ( expr  &&  (op_type) (uint8_t) expr->op == Op_scope )
			{
				return expr->right;
			}
		}
	}
	
	return block;
}

}
