/*
	eval.cc
	-------
*/

#include "vlib/eval.hh"

// vlib
#include "vlib/calc.hh"
#include "vlib/error.hh"
#include "vlib/symbol_table.hh"
#include "vlib/types.hh"


namespace vlib
{
	
	static
	void validate( const Value& value )
	{
		switch ( value.type() )
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
		else if ( v.type() == Value_symbol )
		{
			v = defined( lookup_symbol( v.sym() ) );
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
		
		if ( op == Op_approximate )
		{
			sym->assign( right, true );
			
			return sym->get();
		}
		
		Value& value = modify_symbol( sym );
		
		if ( is_undefined( value ) )
		{
			SYMBOL_ERROR( "update of undefined symbol" );
		}
		
		if ( value.type() != right.type()  &&  right.type() != V_dummy )
		{
			TYPE_ERROR( "update between mixed types not supported" );
		}
		
		if ( value.type() != Value_number )
		{
			TYPE_ERROR( "non-numeric update not supported" );
		}
		
		plus::integer&       a = get_int( value );
		plus::integer const& b = get_int( right );
		
		if ( b.is_zero()  &&  (op == Op_divide_by  ||  op == Op_remain_by) )
		{
			DOMAIN_ERROR( "division by zero" );
		}
		
		const Value& vtype = sym->vtype();
		
		if ( vtype.type() == Value_base_type )
		{
			if ( &vtype.typeinfo() != &integer_vtype )
			{
				Value result;
				
				if ( op == Op_postinc  ||  op == Op_postdec )
				{
					const bool inc = op == Op_postinc;
					
					result = value;
					
					sym->assign( a + (inc ? 1 : -1) );
					
					return result;
				}
				
				switch ( op )
				{
					case Op_preinc:       result = a + 1;  break;
					case Op_predec:       result = a - 1;  break;
					
					case Op_increase_by:  result = a + b;  break;
					case Op_decrease_by:  result = a - b;  break;
					case Op_multiply_by:  result = a * b;  break;
					case Op_divide_by:    result = a / b;  break;
					case Op_remain_by:    result = a % b;  break;
			
					default:
						INTERNAL_ERROR( "no such update assignment operator" );
				}
				
				sym->assign( result );
				
				return value;
			}
		}
		
		switch ( op )
		{
			case Op_postinc:  return a++;
			case Op_postdec:  return a--;
			
			case Op_preinc:  ++a;  break;
			case Op_predec:  --a;  break;
			
			case Op_increase_by:  a += b;  break;
			case Op_decrease_by:  a -= b;  break;
			case Op_multiply_by:  a *= b;  break;
			case Op_divide_by:    a /= b;  break;
			case Op_remain_by:    a %= b;  break;
			
			default:
				INTERNAL_ERROR( "unrecognized update assignment operator" );
		}
		
		return value;
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
			
			if ( op == Op_denote )
			{
				left.sym()->denote( right );
				
				return left;
			}
			
			const Value result = eval_assignment( left.sym(), op, right );
			
			if ( is_symbol_declarator( left )  ||  op != Op_duplicate )
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
