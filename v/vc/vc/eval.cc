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
	
	Value eval( Value    left,
	            op_type  op,
	            Value    right )
	{
		resolve( right );
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
		
		resolve( left );
		validate( left );
		
		return calc( left, op, right );
	}
	
}
