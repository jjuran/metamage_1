/*
	eval.cc
	-------
*/

#include "vc/eval.hh"

// vc
#include "vc/calc.hh"
#include "vc/error.hh"
#include "vc/symbol_table.hh"


namespace vc
{
	
	static
	void validate( const Value& value )
	{
		switch ( value.type )
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
		if ( v.type == Value_undefined )
		{
			SYMBOL_ERROR( "undefined symbol" );
		}
		
		return v;
	}
	
	static
	void resolve( Value& v )
	{
		if ( v.type == Value_symbol_declarator )
		{
			v = Value_nothing;
		}
		else if ( v.type == Value_symbol )
		{
			const symbol_id sym = symbol_id( v.number.clipped() );
			
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
		
		if ( left.type == Value_undefined )
		{
			SYMBOL_ERROR( "update of undefined symbol" );
		}
		
		if ( left.type != right.type )
		{
			TYPE_ERROR( "update between mixed types not supported" );
		}
		
		if ( left.type != Value_number )
		{
			TYPE_ERROR( "non-numeric update not supported" );
		}
		
		plus::integer&       a = left.number;
		plus::integer const& b = right.number;
		
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
			if ( ! is_symbol( left.type ) )
			{
				SYNTAX_ERROR( "left operand of assignment not a symbol" );
			}
			
			const symbol_id sym = symbol_id( left.number.clipped() );
			
			const Value result = eval_assignment( sym, op, right );
			
			if ( left.type == Value_symbol_declarator )
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
