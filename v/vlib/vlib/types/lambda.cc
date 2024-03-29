/*
	lambda.cc
	---------
*/

#include "vlib/types/lambda.hh"

// vlib
#include "vlib/array-utils.hh"
#include "vlib/exceptions.hh"
#include "vlib/peephole.hh"
#include "vlib/return.hh"
#include "vlib/throw.hh"
#include "vlib/dispatch/dispatch.hh"
#include "vlib/dispatch/operators.hh"
#include "vlib/iterators/list_builder.hh"
#include "vlib/iterators/list_iterator.hh"
#include "vlib/types/any.hh"
#include "vlib/types/bareword.hh"


namespace vlib
{
	
	static
	Value apply_prototype_param( const Expr* expr, const Value& v )
	{
		if ( is_empty_list( v ) )
		{
			if ( expr->op == Op_mapping )
			{
				if ( Expr* L_expr = expr->left.expr() )
				{
					expr = L_expr;
				}
			}
			
			if ( expr->op == Op_named_param )
			{
				return expr->right;
			}
		}
		
		return v;
	}
	
	static
	bool multiple( const Expr* expr )
	{
		return expr  &&  expr->op == Op_mapping  &&  is_etc( expr->right );
	}
	
	static
	Value apply_prototype( const Value& prototype, const Value& arguments )
	{
		list_builder result;
		
		list_iterator defs( prototype );
		list_iterator args( arguments );
		
		while ( defs )
		{
			const Value& param = defs.use();
			
			if ( is_etc( param ) )
			{
				if ( defs )
				{
					THROW( "`...` must be last in a prototype" );
				}
				
				result.append( args.rest() );
				
				return result;
			}
			
			Expr* expr = param.expr();
			
			if ( multiple( expr ) )
			{
				result.append( args.rest() );
				
				return result;
			}
			
			const Value& arg = args.use();
			
			const Value r = expr ? apply_prototype_param( expr, arg ) : arg;
			
			if ( ! r )
			{
				THROW( "arguments don't match function prototype" );
			}
			
			result.append( r );
		}
		
		if ( args )
		{
			THROW( "too many arguments" );
		}
		
		return result;
	}
	
	static
	const Value& lambda_prototype( Value& protobody )
	{
		Expr* expr = protobody.expr();
		
		return expr  &&  expr->op == Op_prototype ? expr->left
		                                          : NIL;
	}
	
	static
	Value& lambda_body( Value& protobody )
	{
		Expr* expr = protobody.expr();
		
		return expr  &&  expr->op == Op_prototype ? expr->right
		                                          : protobody;
	}
	
	static
	Value call_function( const Value& f, const Value& arguments )
	{
		if ( const dispatch* methods = f.dispatch_methods() )
		{
			if ( const operators* ops = methods->ops )
			{
				if ( handler_2arg handler = ops->binary )
				{
					const Value result = handler( Op_function, f, arguments );
					
					if ( result )
					{
						return result;
					}
				}
			}
		}
		
		Expr* expr = f.expr();
		
		if ( expr  &&  expr->op == Op_block  &&  expr->right.is_evaluated() )
		{
			return expr->right;
		}
		
		THROW( "attempted call of lambda with invalid body" );
		
		return Value();  // not reached
	}
	
	static
	Value call_lambda( const Value& lambda, const Value& arguments )
	{
		try
		{
			const Value& prototype = lambda_prototype( lambda.expr()->right );
			
			const Value& body = lambda_body( lambda.expr()->right );
			
			return call_function( body, prototype ? apply_prototype( prototype,
			                                                         arguments )
			                                      : arguments );
		}
		catch ( const transfer_via_return& e )
		{
			return e.object;
		}
		catch ( const transfer_via_break& e )
		{
			THROW( "`break` used outside of loop" );
		}
		catch ( const transfer_via_continue& e )
		{
			THROW( "`continue` used outside of loop" );
		}
		
		return Value();
	}
	
	static
	Value parameter_names( const Value& lambda )
	{
		Value& internals = lambda.expr()->right;
		
		if ( const Value& prototype = lambda_prototype( internals ) )
		{
			list_builder names;
			list_iterator defs( prototype );
			
			while ( defs )
			{
				const Value& param = defs.use();
				
				if ( is_etc( param ) )
				{
					break;
				}
				
				const Value* name = &param;
				
				if ( name->expr()  &&  name->expr()->op == Op_mapping )
				{
					name = &name->expr()->left;
				}
				
				if ( name->expr()  &&  name->expr()->op == Op_named_param )
				{
					name = &name->expr()->left;
				}
				
				if ( name->type() == V_str )
				{
					names.append( *name );
				}
			}
			
			return make_array( names );
		}
		
		return empty_list;
	}
	
	static
	Value lambda_member( const Value& obj,
	                     const plus::string& member )
	{
		if ( member == "parameter-names" )
		{
			return parameter_names( obj );
		}
		
		return Value();
	}
	
	static
	Value binary_op_handler( op_type op, const Value& a, const Value& b )
	{
		switch ( op )
		{
			case Op_function:
			case Op_named_unary:
				return call_lambda( a, b );
			
			case Op_member:
				return lambda_member( a, b.as< Member >().get() );
			
			default:
				break;
		}
		
		return Value();
	}
	
	static const operators ops =
	{
		0,  // NULL
		&binary_op_handler,
	};
	
	static const dispatch lambda_dispatch =
	{
		0,  // NULL
		0,  // NULL
		0,  // NULL
		&ops,
	};
	
	Lambda::Lambda( const Value& lambda )
	:
		Value( dummy_operand, Op_lambda, lambda, &lambda_dispatch )
	{
		Value& body = lambda_body( expr()->right );
		
		if ( ! is_block( body ) )
		{
			THROW( "`lambda` requires a block" );
		}
		
		optimize_lambda_body( body );
	}
	
}
