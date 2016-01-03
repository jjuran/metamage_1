/*
	eval.cc
	-------
*/

#include "vlib/eval.hh"

// vlib
#include "vlib/calc.hh"
#include "vlib/error.hh"
#include "vlib/symbol.hh"
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
		if ( v.type() == Value_symbol )
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
	Value eval_assignment( Symbol* sym, op_type op, const Value& right )
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
		plus::integer const& b = right.type() == V_dummy ? a : get_int( right );
		
		if ( b.is_zero()  &&  (op == Op_divide_by  ||  op == Op_remain_by) )
		{
			DOMAIN_ERROR( "division by zero" );
		}
		
		const Value& vtype = sym->vtype();
		
		/*
			The next section of code below explicitly assigns a result (in
			order to check type constraints).  The section below that one
			updates a plus::integer in place instead of creating a new Value.
			This is a problem when the new and old values differ in whether
			they require allocation, since the vbox doesn't get updated to
			match.  If the new result is allocated but the old isn't, then
			the memory will be leaked.  If the old is allocated but the new
			one isn't, then the next access to the refcount will segfault.
			
			As a temporary fix, always go through assign().
		*/
		
		//if ( vtype.type() == Value_base_type )
		{
			//if ( &vtype.typeinfo() != &integer_vtype )
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
		
		// This is temporarily dead code -- see above.
		
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
		if ( op == Op_var  ||  op == Op_const )
		{
			return Value_nothing;
		}
		
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
			
			return eval_assignment( left.sym(), op, right );
		}
		
		resolve( left );
		validate( left );
		
		return calc( left, op, right );
	}
	
}
