/*
	value.cc
	--------
*/

#include "vlib/value.hh"

// iota
#include "iota/swap.hh"


namespace vlib
{
	
	Value::Value( const Value& a, const Value& b )
	:
		type( Value_pair ),
		expr( a, Op_list, b )
	{
	}
	
	Value::Value( value_type type, const Value& a, op_type op, const Value& b )
	:
		type( type ),
		expr( a, op, b )
	{
	}
	
	void swap( Value& a, Value& b )
	{
		using iota::swap;
		
		swap( a.type,     b.type     );
		swap( a.number,   b.number   );
		swap( a.string,   b.string   );
		swap( a.function, b.function );
		swap( a.expr,     b.expr     );
	}
	
	Expr::Expr( const Value& a, op_type op, const Value& b )
	:
		op( op ),
		left( a ),
		right( b )
	{
	}
	
	Value bind_args( const Value& f, const Value& arguments )
	{
		if ( is_empty( arguments ) )
		{
			return f;
		}
		
		if ( Expr* expr = f.expr.get() )
		{
			return bind_args( expr->left, Value( expr->right, arguments ) );
		}
		
		return Value( Value_pair, f, Op_bind_args, arguments );
	}
	
	unsigned long area( const Expr& expr )
	{
		return + sizeof (Expr) - 2 * sizeof (Value)
		       + area( expr.left  )
		       + area( expr.right );
	}
	
}
