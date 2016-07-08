/*
	eval.cc
	-------
*/

#include "vlib/eval.hh"

// plus
#include "plus/integer.hh"

// vlib
#include "vlib/array-utils.hh"
#include "vlib/calc.hh"
#include "vlib/error.hh"
#include "vlib/exceptions.hh"
#include "vlib/list-utils.hh"
#include "vlib/symbol.hh"
#include "vlib/throw.hh"
#include "vlib/tracker.hh"
#include "vlib/types.hh"
#include "vlib/types/integer.hh"


namespace vlib
{
	
	static
	void validate( const Value& value )
	{
		switch ( value.type() )
		{
			case Value_nothing:
				THROW( "invalid void state during evaluation" );
			
			default:
				break;
		}
	}
	
	static
	const Value& defined( const Value& v )
	{
		if ( is_undefined( v ) )
		{
			THROW( "undefined symbol" );
		}
		
		return v;
	}
	
	const Value& eval( const Value& v )
	{
		if ( v.type() == Value_symbol )
		{
			return defined( lookup_symbol( v.sym() ) );
		}
		
		return v;
	}
	
	static
	Value eval_assignment( const Value& left, op_type op, const Value& right )
	{
		Symbol* sym = left.sym();
		
		if ( op == Op_duplicate )
		{
			sym->assign( right, false );
			
			return right;
		}
		
		if ( op == Op_approximate )
		{
			sym->assign( right, true );
			
			return sym->get();
		}
		
		if ( op == Op_push )
		{
			push( left, right );
			
			return sym->get();
		}
		
		Value& value = modify_symbol( sym );
		
		if ( is_undefined( value ) )
		{
			THROW( "update of undefined symbol" );
		}
		
		if ( value.type() != right.type()  &&  right.type() != V_dummy )
		{
			THROW( "update between mixed types not supported" );
		}
		
		if ( value.type() != Value_number )
		{
			THROW( "non-numeric update not supported" );
		}
		
		plus::integer&       a = value.number();
		plus::integer const& b = right.type() == V_dummy ? a : right.number();
		
		if ( b.is_zero()  &&  (op == Op_divide_by  ||  op == Op_remain_by) )
		{
			THROW( "division by zero in update" );
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
				if ( op == Op_postinc  ||  op == Op_postdec )
				{
					const bool inc = op == Op_postinc;
					
					Value result = value;
					
					sym->assign( Integer( a + (inc ? 1 : -1) ) );
					
					return result;
				}
				
				plus::integer result;
				
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
				
				sym->assign( Integer( result ) );
				
				return value;
			}
		}
		
		// This is temporarily dead code -- see above.
		
		switch ( op )
		{
			case Op_postinc:  return Integer( a++ );
			case Op_postdec:  return Integer( a-- );
			
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
	
	static
	bool is_symbolic( const Value& v )
	{
		if ( is_symbol( v ) )
		{
			return true;
		}
		
		if ( Expr* expr = v.listexpr() )
		{
			return is_symbolic( expr->left );
		}
		
		return false;
	}
	
	static
	Value eval_part_2( const Value&  left,
	                   op_type       op,
	                   const Value&  right );
	
	Value eval( const Value&        left,
	            op_type             op,
	            const Value&        right,
	            const source_spec&  source )
	{
		if ( op == Op_var  ||  op == Op_const )
		{
			return Value_nothing;
		}
		
		if ( op == Op_list )
		{
			if ( is_symbolic( left )  &&  is_symbolic( right ) )
			{
				return make_list( left, right );
			}
		}
		
		try
		{
			validate( right );
			
			if ( ! is_left_varop( op ) )
			{
				validate( left );
				
				return calc( left, op, right );
			}
			
			return eval_part_2( left, op, right );
		}
		catch ( const exception& e )
		{
			throw user_exception( e.message, source );
		}
		catch ( const user_exception& e )
		{
			if ( e.source.empty() )
			{
				throw user_exception( e.object, source );
			}
			
			throw;
		}
		catch ( const transfer_via_break& e )
		{
			if ( e.source.empty() )
			{
				throw transfer_via_break( source );
			}
			
			throw;
		}
		catch ( const transfer_via_continue& e )
		{
			if ( e.source.empty() )
			{
				throw transfer_via_continue( source );
			}
			
			throw;
		}
		catch ( const transfer_via_return& e )
		{
			if ( e.source.empty() )
			{
				throw transfer_via_return( e.object, source );
			}
			
			throw;
		}
		
		// Silence Metrowerks warning
		return Value();
	}
	
	Value eval_part_2( const Value&  left,
	                   op_type       op,
	                   const Value&  right )
	{
		{
			if ( Expr* ax = left.listexpr() )
			{
				if ( Expr* bx = right.listexpr() )
				{
					eval( ax->left,  op, bx->left,  source_spec() );
					eval( ax->right, op, bx->right, source_spec() );
					
					return right;
				}
				
				THROW( "too few values in list assignment" );
			}
			
			if ( ! is_symbol( left ) )
			{
				THROW( "left operand of assignment not a symbol" );
			}
			
			if ( op == Op_denote )
			{
				left.sym()->denote( right );
				
				return left;
			}
			
			track_symbol( left, right );
			
			try
			{
				return eval_assignment( left, op, right );
			}
			catch ( const plus::ibox::limb_count_overflow& )
			{
				THROW( "bigint overflow" );
			}
		}
		
		return Value();  // not reached
	}
	
}
