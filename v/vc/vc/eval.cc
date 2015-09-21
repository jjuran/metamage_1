/*
	eval.cc
	-------
*/

#include "vc/eval.hh"

// vc
#include "vc/error.hh"
#include "vc/function_id.hh"


namespace vc
{
	
	static
	const plus::integer& nonzero( const plus::integer& x )
	{
		if ( x.is_zero() )
		{
			DOMAIN_ERROR( "division by zero" );
		}
		
		return x;
	}
	
	static
	Value eval_function( const plus::integer& f, Value arg )
	{
		switch ( f.clipped() )
		{
			case Function_abs:   arg.number.absolve();  break;
			case Function_half:  arg.number.halve();    break;
			
			default:
				INTERNAL_ERROR( "unimplemented function" );
		}
		
		return arg;
	}
	
	static
	Value eval_unary( op_type op, const Value& v )
	{
		switch ( v.type )
		{
			case Value_boolean:
			case Value_number:
				switch ( op )
				{
					case Op_unary_plus:   return  v.number;
					case Op_unary_minus:  return -v.number;
					
					default:  break;
				}
				
			default:
				break;
		}
		
		INTERNAL_ERROR( "unsupported operator in eval_unary()" );
		
		return Value();
	}
	
	static
	Value eval_bool( const plus::integer&  left,
	                 op_type               op,
	                 const plus::integer&  right )
	{
		SYNTAX_ERROR( "operator not defined for boolean values" );
		
		return 0;
	}
	
	static
	Value eval( const plus::integer&  left,
	            op_type               op,
	            const plus::integer&  right )
	{
		switch ( op )
		{
			case Op_add:       return left + right;
			case Op_subtract:  return left - right;
			case Op_multiply:  return left * right;
			case Op_divide:    return left / nonzero( right );
			case Op_remain:    return left % nonzero( right );
			case Op_modulo:    return modulo( left, nonzero( right ) );
			
			case Op_empower:   return raise_to_power( left, right );
			
			default:
				break;
		}
		
		INTERNAL_ERROR( "unsupported operator in eval()" );
		
		return 0;
	}
	
	Value eval( const Value&  left,
	            op_type       op,
	            const Value&  right )
	{
		if ( left.type == Value_none )
		{
			return eval_unary( op, right );
		}
		
		if ( op == Op_function )
		{
			return eval_function( left.number, right );
		}
		
		if ( left.type == right.type )
		{
			switch ( left.type )
			{
				case Value_boolean:
					return eval_bool( left.number, op, right.number );
				
				case Value_number:
					return eval( left.number, op, right.number );
				
				default:
					break;
			}
		}
		
		SYNTAX_ERROR( "operator not defined on mixed types" );
		
		return Value();
	}
	
}
