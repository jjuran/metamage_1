/*
	eval.cc
	-------
*/

#include "vc/eval.hh"

// vc
#include "vc/error.hh"


namespace vc
{
	
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
			
			default:
				break;
		}
		
		INTERNAL_ERROR( "unsupported operator in eval()" );
		
		return 0;
	}
	
}
