/*
	equal.cc
	--------
*/

#include "vlib/equal.hh"

// bignum
#include "bignum/integer.hh"

// vlib
#include "vlib/error.hh"
#include "vlib/is_type.hh"
#include "vlib/throw.hh"
#include "vlib/dispatch/compare.hh"
#include "vlib/dispatch/dispatch.hh"
#include "vlib/iterators/list_iterator.hh"
#include "vlib/types/boolean.hh"


namespace vlib
{
	
	static
	bool equal_atoms( const Value& a, const Value& b )
	{
		if ( a.type() != b.type() )
		{
			return false;
		}
		
		if ( a.dispatch_methods() != b.dispatch_methods() )
		{
			return false;
		}
		
		if ( const dispatch* methods = a.dispatch_methods() )
		{
			if ( const comparison* cmp = methods->compare )
			{
				if ( const equality equ = cmp->equal )
				{
					return equ( a, b );
				}
				
				if ( const ordering ord = cmp->order )
				{
					return ord( a, b ) == 0;
				}
			}
		}
		
		switch ( a.type() )
		{
			case Value_NIL:
			case Value_nothing:
			case Value_dummy_operand:
			case Value_empty_list:
			case Value_empty_array:
				return true;
			
			case Value_symbol:
				return a.sym() == b.sym();
			
			case Value_boolean:
				return !!(const Boolean&) a == !!(const Boolean&) b;
			
			case Value_pair:
				{
					Expr& ax = *a.expr();
					Expr& bx = *b.expr();
					
					if ( ax.op != bx.op )
					{
						return false;
					}
					
					const bool compare_left = (ax.op & 0xFF) != Op_scope;
					
					if ( compare_left  &&  ! equal_atoms( ax.left, bx.left ) )
					{
						return false;
					}
					
					return equal_atoms( ax.right, bx.right );
				}
			
			default:
				INTERNAL_ERROR( "unsupported type in equal_atoms()" );
				break;
		}
		
		return false;
	}
	
	bool single_type_mismatch( const Value& one, const Value& two )
	{
		if ( ! is_single( one )  ||  ! is_single( two ) )
		{
			return false;
		}
		
		if ( is_type( one )  &&  is_type( two ) )
		{
			return false;
		}
		
		if ( one.type() != two.type() )
		{
			return true;
		}
		
		if ( one.dispatch_methods() != two.dispatch_methods() )
		{
			return true;
		}
		
		if ( one.type() == Value_pair )
		{
			Expr& ax = *one.expr();
			Expr& bx = *two.expr();
			
			return ax.op != bx.op;
		}
		
		return false;
	}
	
	bool equal( const Value& one, const Value& two )
	{
		if ( single_type_mismatch( one, two ) )
		{
			THROW( "mismatched types in equality relation" );
		}
		
		list_iterator a( one );
		list_iterator b( two );
		
		while ( a  ||  b )
		{
			if ( ! equal_atoms( a.use(), b.use() ) )
			{
				return false;
			}
		}
		
		return true;
	}
	
}
