/*
	fold.cc
	-------
*/

#include "vlib/fold.hh"

// vlib
#include "vlib/assert.hh"
#include "vlib/exceptions.hh"
#include "vlib/execute.hh"
#include "vlib/pure.hh"
#include "vlib/return.hh"
#include "vlib/scope.hh"
#include "vlib/symbol.hh"
#include "vlib/throw.hh"
#include "vlib/types/boolean.hh"
#include "vlib/types/string.hh"
#include "vlib/types/symdesc.hh"


namespace vlib
{
	
	enum op_foldability
	{
		Fold_never         = -1,
		Fold_always        = 0,
		Fold_if_pure_left  = 1,
		Fold_if_pure_right = 2,
		Fold_if_pure_both  = 3,
	};
	
	static inline
	bool unconditional( op_foldability foldability )
	{
		return foldability <= 0;
	}
	
	static
	op_foldability foldability_of( op_type op )
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
			case Op_gamut:
				return Fold_always;
			
			case Op_named_unary:
			case Op_function:
				return Fold_if_pure_left;
			
			case Op_map:
			case Op_ver:
			case Op_per:
			case Op_lambda:
				return Fold_if_pure_right;
			
			default:
				break;
		}
		
		return Fold_never;
	}
	
	static
	bool is_constant_op( op_type op )
	{
		switch ( op )
		{
			case Op_empower:
			case Op_forward_init:
			case Op_reverse_init:
			case Op_array:
			case Op_multiply:
			case Op_divide:
			case Op_percent:
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
		if ( v.is_evaluated() )
		{
			return true;
		}
		
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
	bool is_block_or_constant( const Value& v )
	{
		if ( Expr* expr = v.expr() )
		{
			if ( expr->op == Op_block  ||  expr->op == Op_lambda )
			{
				return true;
			}
		}
		
		return is_constant( v );
	}
	
	static
	bool is_pure_block( const Value& v )
	{
		if ( Expr* expr = v.expr() )
		{
			if ( expr->op == Op_block  ||  expr->op == Op_prototype )
			{
				return is_pure( v );
			}
		}
		
		return false;
	}
	
	static
	bool is_pure_block_def( const Value& a, op_type op, const Value& b )
	{
		if ( op == Op_function )
		{
			if ( Expr* expr = a.expr() )
			{
				return expr->op == Op_def  &&  is_pure_block( b );
			}
		}
		
		return false;
	}
	
	static
	bool is_foldable( const Value& a, op_type op, const Value& b )
	{
		const op_foldability foldability = foldability_of( op );
		
		if ( unconditional( foldability ) )
		{
			return foldability == Fold_always;
		}
		
		if ( foldability & Fold_if_pure_left  &&  ! is_pure( a ) )
		{
			return false;
		}
		
		if ( foldability & Fold_if_pure_right  &&  ! is_pure( b ) )
		{
			return false;
		}
		
		return true;
	}
	
	static
	Value subfold( const Value& v )
	{
		try
		{
			return execute( Value( empty_list, Op_scope, v ) );
		}
		catch ( const user_exception& )
		{
		}
		catch ( const transfer_via_break& )
		{
		}
		catch ( const transfer_via_continue& )
		{
		}
		catch ( const transfer_via_return& )
		{
		}
		
		return NIL;
	}
	
	static
	Value make_constant( const Value&    decl,
	                     const Value*    type,
	                     const Value&    value,
	                     lexical_scope*  scope,
	                     bool            coercive = false )
	{
		Symbol* sym = decl.decl_sym();
		
		const int i = ((const SymDesc&) sym->get()).index();
		
		if ( scope )
		{
			scope->immortalize_constant( i, value );
		}
		
		sym->deref_unsafe() = undefined;
		
		if ( type )
		{
			sym->denote( *type );
		}
		
		sym->assign( value, coercive );
		
		return sym->get();
	}
	
	static
	Value inner_fold( const Value& v, lexical_scope* scope )
	{
		Expr* expr = v.expr();
		
		const Value& a = expr->left;
		const Value& b = expr->right;
		
		const op_type op = expr->op;
		
		if ( op == Op_list )
		{
			if ( is_empty_list( a ) )  return b;
			if ( is_empty_list( b ) )  return a;
		}
		
		if ( is_foldable( a, op, b ) )
		{
			if ( is_pure_block_def( a, op, b ) )
			{
				return make_constant( a, NULL, Value( Op_lambda, b ), scope );
			}
			
			if ( is_block_or_constant( a )  &&  is_block_or_constant( b ) )
			{
				const Value folded = subfold( v );
				
				if ( folded )
				{
					return folded;
				}
			}
		}
		else if ( op == Op_end )
		{
			if ( is_block_or_constant( a ) )
			{
				return b;
			}
		}
		else if ( op == Op_do )
		{
			Expr* expr = b.expr();
			
			if ( expr  &&  expr->op == Op_block )
			{
				const Value& body = expr->right.expr()->right;
				
				if ( is_constant( body ) )
				{
					return body;
				}
			}
		}
		else if ( op == Op_assert )
		{
			if ( is_constant( b ) )
			{
				check_assertion_result( b, b, source_spec() );
				
				return nothing;  // Assertion passed!
			}
		}
		else if ( op == Op_and  ||  op == Op_or )
		{
			if ( is_constant( a ) )
			{
				const Boolean truth = a.to< Boolean >();
				
				return truth == (op == Op_or) ? a : b;
			}
		}
		else if ( op == Op_duplicate  ||  op == Op_approximate )
		{
			if ( is_pure_block( b )  ||  is_constant( b ) )
			{
				const Value* decl = NULL;
				const Value* type = NULL;
				
				if ( is_symbol_declarator( a ) )
				{
					decl = &a;
				}
				else if ( is_type_annotation( a ) )
				{
					type = &a.expr()->right;
					
					if ( is_constant( *type ) )
					{
						decl = &a.expr()->left;
					}
				}
				
				if ( decl  &&  decl->expr()->op == Op_const )
				{
					const bool coercive = op == Op_approximate;
					
					return make_constant( *decl, type, b, scope, coercive );
				}
			}
		}
		
		return NIL;
	}
	
	Value fold( const Value& v, lexical_scope* scope )
	{
		if ( Expr* expr = v.expr() )
		{
			try
			{
				return inner_fold( v, scope );
			}
			catch ( const exception& e )
			{
				throw user_exception( String( e.message ), expr->source );
			}
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
