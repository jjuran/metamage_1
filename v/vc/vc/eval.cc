/*
	eval.cc
	-------
*/

#include "vc/eval.hh"

// vc
#include "vc/error.hh"
#include "vc/pure.hh"
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
	
	Value eval( const Value&  left,
	            op_type       op,
	            const Value&  right )
	{
		validate( left  );
		validate( right );
		
		if ( op == Op_duplicate )
		{
			if ( ! is_symbol( left.type ) )
			{
				SYNTAX_ERROR( "left operand of assignment not a symbol" );
			}
			
			assign_symbol( symbol_id( left.number.clipped() ), right );
			
			if ( left.type == Value_symbol_declarator )
			{
				return right;
			}
			
			return Value();
		}
		
		return eval_pure( left, op, right );
	}
	
}
