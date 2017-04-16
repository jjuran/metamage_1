/*
	fold.cc
	-------
*/

#include "vlib/fold.hh"

// vlib
#include "vlib/calc.hh"
#include "vlib/symbol.hh"


namespace vlib
{
	
	static
	bool is_pure( op_type op )
	{
		switch ( op )
		{
			case Op_empower:
			case Op_typeof:
			case Op_unary_plus:
			case Op_unary_minus:
			case Op_unary_negate:
			case Op_unary_count:
			case Op_unary_deref:
			case Op_array:
			case Op_subscript:
			case Op_member:
			case Op_multiply:
			case Op_divide:
			case Op_percent:
			case Op_modulo:
			case Op_add:
			case Op_subtract:
			case Op_intersection:
			case Op_exclusion:
			case Op_union:
			case Op_in:
			case Op_not:
			case Op_isa:
			case Op_equal:
			case Op_unequal:
			case Op_lt:
			case Op_lte:
			case Op_gt:
			case Op_gte:
			case Op_cmp:
				return true;
			
			default:
				break;
		}
		
		return false;
	}
	
	static
	bool is_constant_op( op_type op )
	{
		switch ( op )
		{
			case Op_divide:
			case Op_empower:
			case Op_mapping:
			case Op_gamut:
			case Op_delta:
			case Op_list:
				return true;
			
			default:
				break;
		}
		
		return false;
	}
	
	static
	bool is_constant( const Value& v )
	{
		if ( Expr* expr = v.expr() )
		{
			if ( expr->op == Op_list  &&  expr->left.listexpr() )
			{
				return false;
			}
			
			return is_constant_op( expr->op    )  &&
			       is_constant   ( expr->left  )  &&
			       is_constant   ( expr->right );
		}
		
		return v.type() != Value_symbol;
	}
	
	static
	Value subfold( const Value& a, op_type op, const Value& b )
	{
		if ( is_constant( a )  &&  is_constant( b ) )
		{
			if ( a.type() == V_dummy )
			{
				return calc( b, op, a );
			}
			
			return calc( a, op, b );
		}
		
		return NIL;
	}
	
	static
	Value fold( const Value& a, op_type op, const Value& b )
	{
		if ( is_pure( op ) )
		{
			const Value folded = subfold( a, op, b );
			
			if ( folded.type() )
			{
				return folded;
			}
		}
		else if ( op == Op_end )
		{
			if ( is_constant( a ) )
			{
				return b;
			}
		}
		else if ( op == Op_do )
		{
			Expr* expr = b.expr();
			
			if ( expr->op == Op_block )
			{
				const Value& body = expr->right.expr()->right;
				
				if ( is_constant( body ) )
				{
					return body;
				}
			}
		}
		
		return NIL;
	}
	
	Value fold( const Value& v )
	{
		if ( Expr* expr = v.expr() )
		{
			return fold( expr->left, expr->op, expr->right );
		}
		
		if ( const Symbol* sym = v.sym() )
		{
			if ( sym->is_immutable() )
			{
				const Value& x = sym->get();
				
				if ( x.type() != V_desc )
				{
					return x;
				}
			}
		}
		
		return NIL;
	}
	
}
