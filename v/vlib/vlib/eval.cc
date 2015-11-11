/*
	eval.cc
	-------
*/

#include "vlib/eval.hh"

// vlib
#include "vlib/calc.hh"
#include "vlib/error.hh"
#include "vlib/symbol_table.hh"


namespace vlib
{
	
	static
	void validate( const Value& value )
	{
		switch ( get_type( value ) )
		{
			case Value_nothing:
				SYNTAX_ERROR( "invalid void state during evaluation" );
			
			default:
				break;
		}
	}
	
	static
	const Value& defined( const Value& v )
	{
		if ( is_undefined( v ) )
		{
			SYMBOL_ERROR( "undefined symbol" );
		}
		
		return v;
	}
	
	static
	void resolve( Value& v )
	{
		if ( is_symbol_declarator( v ) )
		{
			v = Value_nothing;
		}
		else if ( get_type( v ) == Value_symbol )
		{
			const symbol_id sym = symbol_id( get_int( v ).clipped() );
			
			v = defined( lookup_symbol( sym ) );
		}
	}
	
	Value eval( Value v )
	{
		resolve( v );
		
		return v;
	}
	
	static
	Value eval_assignment( symbol_id sym, op_type op, const Value& right )
	{
		if ( op == Op_duplicate )
		{
			assign_symbol( sym, right );
			
			return right;
		}
		
		Value& left = modify_symbol( sym );
		
		if ( is_undefined( left ) )
		{
			SYMBOL_ERROR( "update of undefined symbol" );
		}
		
		if ( get_type( left ) != get_type( right ) )
		{
			TYPE_ERROR( "update between mixed types not supported" );
		}
		
		if ( get_type( left ) != Value_number )
		{
			TYPE_ERROR( "non-numeric update not supported" );
		}
		
		plus::integer&       a = get_int( left  );
		plus::integer const& b = get_int( right );
		
		if ( b.is_zero()  &&  (op == Op_divide_by  ||  op == Op_remain_by) )
		{
			DOMAIN_ERROR( "division by zero" );
		}
		
		switch ( op )
		{
			case Op_increase_by:  a += b;  break;
			case Op_decrease_by:  a -= b;  break;
			case Op_multiply_by:  a *= b;  break;
			case Op_divide_by:    a /= b;  break;
			case Op_remain_by:    a %= b;  break;
			
			default:
				INTERNAL_ERROR( "unrecognized update assignment operator" );
		}
		
		return left;
	}
	
	Value eval( Value    left,
	            op_type  op,
	            Value    right )
	{
		resolve( right );
		validate( right );
		
		if ( is_left_varop( op ) )
		{
			if ( ! is_symbol( left ) )
			{
				SYNTAX_ERROR( "left operand of assignment not a symbol" );
			}
			
			const symbol_id sym = symbol_id( get_int( left ).clipped() );
			
			const Value result = eval_assignment( sym, op, right );
			
			if ( is_symbol_declarator( left ) )
			{
				return result;
			}
			
			return Value();
		}
		
		resolve( left );
		validate( left );
		
		return calc( left, op, right );
	}
	
}
