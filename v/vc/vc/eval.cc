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
	plus::integer eval_function( const plus::integer& f, plus::integer arg )
	{
		if ( f == Function_abs )
		{
			arg.absolve();
		}
		else if ( f == Function_half )
		{
			arg.halve();
		}
		else
		{
			INTERNAL_ERROR( "unimplemented function" );
		}
		
		return arg;
	}
	
	plus::integer eval( const plus::integer&  left,
	                    op_type               op,
	                    const plus::integer&  right )
	{
		switch ( op )
		{
			case Op_unary_plus:   return  right;
			case Op_unary_minus:  return -right;
			
			case Op_add:       return left + right;
			case Op_subtract:  return left - right;
			case Op_multiply:  return left * right;
			case Op_divide:    return left / nonzero( right );
			case Op_remain:    return left % nonzero( right );
			case Op_modulo:    return modulo( left, nonzero( right ) );
			
			case Op_empower:   return raise_to_power( left, right );
			
			case Op_function:  return eval_function( left, right );
			
			default:
				break;
		}
		
		INTERNAL_ERROR( "unsupported operator in eval()" );
		
		return 0;
	}
	
}
