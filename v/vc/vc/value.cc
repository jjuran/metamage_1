/*
	value.cc
	--------
*/

#include "vc/value.hh"

// iota
#include "iota/swap.hh"


namespace vc
{
	
	Value::Value( const Value& a, const Value& b )
	:
		type( Value_pair ),
		expr( a, Op_list, b )
	{
	}
	
	void swap( Value& a, Value& b )
	{
		using iota::swap;
		
		swap( a.type,   b.type   );
		swap( a.number, b.number );
		swap( a.string, b.string );
		swap( a.expr,   b.expr   );
	}
	
	Expr::Expr( const Value& a, op_type op, const Value& b )
	:
		op( op ),
		left( a ),
		right( b )
	{
	}
	
}
