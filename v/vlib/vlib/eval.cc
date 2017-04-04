/*
	eval.cc
	-------
*/

#include "vlib/eval.hh"

// vlib
#include "vlib/array-utils.hh"
#include "vlib/calc.hh"
#include "vlib/collectible.hh"
#include "vlib/error.hh"
#include "vlib/exceptions.hh"
#include "vlib/list-utils.hh"
#include "vlib/symbol.hh"
#include "vlib/targets.hh"
#include "vlib/throw.hh"
#include "vlib/tracker.hh"
#include "vlib/types.hh"
#include "vlib/dispatch/dispatch.hh"
#include "vlib/dispatch/operators.hh"
#include "vlib/iterators/list_iterator.hh"
#include "vlib/types/string.hh"


namespace vlib
{
	
	static
	void validate( const Value& value )
	{
		switch ( value.type() )
		{
			case Value_NIL:
				THROW( "invalid NIL state during evaluation" );
			
			case Value_nothing:
				THROW( "invalid nothing state during evaluation" );
			
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
	const Symbol* target_sym( const Value& v )
	{
		if ( Expr* expr = v.expr() )
		{
			if ( expr->op == Op_subscript )
			{
				return target_sym( expr->left );
			}
		}
		
		return v.sym();
	}
	
	static
	bool check_target_sym( const Value& v )
	{
		if ( const Symbol* sym = target_sym( v ) )
		{
			return sym->check_type_invariant();
		}
		
		return false;
	}
	
	static
	void check_type( const Value& type, const Value& v )
	{
		if ( is_undefined( v ) )
		{
			THROW( "undefined value" );
		}
		
		if ( type.type()  &&  ! as_assigned( type, v ).type() )
		{
			THROW( "type mismatch in assignment" );
		}
	}
	
	static
	void track_symbol_if_collectible( const Value& symbol, const Value& v )
	{
		if ( symbol_is_collectible( *symbol.sym(), v ) )
		{
			track_symbol( symbol, v );
		}
	}
	
	static
	Value eval_assignment( const Value& left, op_type op, const Value& right )
	{
		handler_into handler = 0;  // NULL
		
		Target target = make_target( left );
		
		if ( const dispatch* methods = target.addr->dispatch_methods() )
		{
			if ( const operators* ops = methods->ops )
			{
				handler = ops->mutating;
			}
		}
		
		if ( target.type == 0 )  // NULL
		{
			/*
				The left operand is a packed structure, whose elements can't
				be targeted directly.
			*/
			
			if ( handler )
			{
				const Value result = handler( op, target, left, right );
				
				if ( result.type() )
				{
					return result;
				}
			}
			
			THROW( "can't target left operand" );
		}
		
		if ( op == Op_duplicate  ||  op == Op_approximate )
		{
			if ( is_symbol( left ) )
			{
				track_symbol_if_collectible( left, right );
			}
			
			const bool collectible = target_is_collectible( target );
			
			const bool coercive = op == Op_approximate;
			
			assign( target, right, coercive, target_sym( left ) );
			
			if ( collectible )
			{
				cull_unreachable_objects();
			}
			
			return *target.addr;
		}
		
		if ( op == Op_take )
		{
			if ( left.sym() == 0 )  // NULL
			{
				THROW( "can't take from a container element" );
			}
			
			const Value taken = *target.addr;
			
			*target.addr = Value_undefined;
			
			return taken;
		}
		
		if ( is_right_varop( op ) )
		{
			Target second = make_target( right );
			
			check_type( *target.type, *second.addr );
			
			if ( op == Op_swap )
			{
				check_type( *second.type, *target.addr );
			}
			
			if ( op == Op_move )
			{
				if ( right.sym() == 0 )  // NULL
				{
					THROW( "can't move from a container element" );
				}
			}
			
			if ( Symbol* sym = left.sym() )
			{
				track_symbol_if_collectible( left, *second.addr );
			}
			
			if ( op == Op_swap  &&  right.sym() )
			{
				track_symbol_if_collectible( right, *target.addr );
			}
			
			swap( *target.addr, *second.addr );
			
			const char* error = "type mismatch in swap";
			
			if ( ! check_target_sym( left ) )
			{
				if ( op == Op_move )
				{
					error = "type mismatch in move";
				}
			}
			else if ( op == Op_move  ||  check_target_sym( right ) )
			{
				error = 0;  // NULL
			}
			
			if ( error )
			{
				swap( *target.addr, *second.addr );
				
				THROW( error );
			}
			
			if ( op == Op_move )
			{
				target.addr = second.addr;
				
				const bool collectible = target_is_collectible( target );
				
				*second.addr = Value_undefined;
				
				if ( collectible )
				{
					cull_unreachable_objects();
				}
				
				return right;
			}
			
			return Value();
		}
		
		if ( op == Op_push )
		{
			push( target, right );
			
			return *target.addr;
		}
		
		Value& value = *target.addr;
		
		if ( is_undefined( value ) )
		{
			THROW( "update of undefined symbol" );
		}
		
		if ( right.type() == V_dummy )
		{
			if ( const dispatch* methods = target.addr->dispatch_methods() )
			{
				if ( const operators* ops = methods->ops )
				{
					if ( handler_step handler = ops->advance )
					{
						const Value result = handler( op, target );
						
						if ( result.type() )
						{
							return result;
						}
					}
				}
			}
		}
		
		if ( handler != 0 )  // NULL
		{
			const Value result = handler( op, target, left, right );
			
			if ( result.type() )
			{
				return result;
			}
		}
		
		THROW( "unimplemented update assignment" );
		
		return Value();
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
		catch ( const plus::ibox::limb_count_overflow& )
		{
			throw user_exception( String( "bigint overflow" ), source );
		}
		catch ( const exception& e )
		{
			throw user_exception( String( e.message ), source );
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
				list_iterator a( left  );
				list_iterator b( right );
				
				while ( b )
				{
					if ( ! a )
					{
						THROW( "too many values in list assignment" );
					}
					
					eval( a.use(), op, b.use(), source_spec() );
				}
				
				if ( a.finished() )
				{
					return right;
				}
				
				THROW( "too few values in list assignment" );
			}
			
			if ( op == Op_denote )
			{
				left.sym()->denote( right );
				
				return left;
			}
			
			return eval_assignment( left, op, right );
		}
		
		return Value();  // not reached
	}
	
}
