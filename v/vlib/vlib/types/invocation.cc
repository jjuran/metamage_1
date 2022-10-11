/*
	invocation.cc
	-------------
*/

#include "vlib/types/invocation.hh"

// relix-compat
#include "relix/recurse.hh"

// vlib
#include "vlib/dispatch/dispatch.hh"
#include "vlib/dispatch/operators.hh"
#include "vlib/types/proc.hh"


namespace vlib
{
	
	static
	void recursive_call( function_type invoke, const Value& f, Value& result )
	{
		result = invoke( f );
	}
	
	static
	Value recurse( function_type f, const Value& v )
	{
		using relix::recurse;
		
		typedef function_type  F;
		typedef const Value&   V;
		typedef       Value&   R;
		
		Value result;
		
		recurse< void, F, F, V, V, R, R >( &recursive_call, f, v, result );
		
		return result;
	}
	
	static
	Value invoke_block( const Value& invocation, const Value& arguments )
	{
		Expr* expr = invocation.expr();
		
		const Value& invoke = expr->left;
		const Value& block  = expr->right;
		
		Value bound_block( block, Op_bind_args, arguments );
		
		return recurse( invoke.as< Proc >().addr(), bound_block );
	}
	
	const Value& dereference_block( const Value& invocation )
	{
		const Value& entry = invocation.expr()->right.expr()->right;
		
		if ( entry.is_evaluated() )
		{
			return entry;
		}
		
		return NIL;
	}
	
	static
	Value unary_op_handler( op_type op, const Value& v )
	{
		switch ( op )
		{
			case Op_unary_deref:
				return dereference_block( v );
			
			default:
				break;
		}
		
		return NIL;
	}
	
	static
	Value binary_op_handler( op_type op, const Value& a, const Value& b )
	{
		switch ( op )
		{
			case Op_function:
			case Op_named_unary:
				return invoke_block( a, b );
			
			default:
				break;
		}
		
		return Value();
	}
	
	static const operators ops =
	{
		&unary_op_handler,
		&binary_op_handler,
	};
	
	const dispatch invocation_dispatch =
	{
		0,  // NULL
		0,  // NULL
		0,  // NULL
		&ops,
	};
	
}
