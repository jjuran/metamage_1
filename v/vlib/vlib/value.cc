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
		its_type( Value_pair ),
		its_expr( a, Op_list, b )
	{
	}
	
	Value::Value( value_type type, const Value& a, op_type op, const Value& b )
	:
		its_type( type ),
		its_expr( a, op, b )
	{
	}
	
	void Value::swap( Value& that )
	{
		using iota::swap;
		
		swap( its_type, that.its_type );
		swap( its_int,  that.its_int  );
		swap( its_str,  that.its_str  );
		swap( its_proc, that.its_proc );
		swap( its_expr, that.its_expr );
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
		
		if ( Expr* expr = get_expr( f ) )
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
