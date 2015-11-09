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
		its_box( Value_pair ),
		its_expr( a, Op_list, b )
	{
	}
	
	Value::Value( value_type type, const Value& a, op_type op, const Value& b )
	:
		its_box( type ),
		its_expr( a, op, b )
	{
	}
	
	void Value::swap( Value& that )
	{
		using iota::swap;
		
		swap( its_box,  that.its_box  );
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
	
	static
	unsigned long area( const Expr& expr )
	{
		return + sizeof (Expr) - 2 * sizeof (Value)
		       + area( expr.left  )
		       + area( expr.right );
	}
	
	static
	unsigned long area( const expr_box& box )
	{
		if ( Expr* expr = box.get() )
		{
			return sizeof (expr_box) + area( *expr );
		}
		
		return sizeof (expr_box);
	}
	
	unsigned long area( const Value& v )
	{
		return + sizeof (Value)
		       - sizeof v.its_box
		       - sizeof v.its_expr
		       + area( v.its_box  )
		       + area( v.its_expr );
	}
	
}
