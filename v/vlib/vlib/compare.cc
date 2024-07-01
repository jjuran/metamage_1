/*
	compare.cc
	----------
*/

#include "vlib/compare.hh"

// vlib
#include "vlib/equal.hh"
#include "vlib/error.hh"
#include "vlib/throw.hh"
#include "vlib/value.hh"
#include "vlib/dispatch/compare.hh"
#include "vlib/dispatch/dispatch.hh"
#include "vlib/iterators/list_iterator.hh"
#include "vlib/types/boolean.hh"


namespace vlib
{
	
	using math::integer::cmp_t;
	
	
	static
	cmp_t compare_atoms( const Value& a, const Value& b )
	{
		if ( a.type() != b.type() )
		{
			THROW( "can't compare atoms of different types" );
		}
		
		if ( a.dispatch_methods() != b.dispatch_methods() )
		{
			THROW( "can't compare atoms of different types" );
		}
		
		if ( const dispatch* methods = a.dispatch_methods() )
		{
			if ( const comparison* cmp = methods->compare )
			{
				if ( const ordering ord = cmp->order )
				{
					return ord( a, b );
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
				return 0;  // equal
			
			case Value_boolean:
				return !!(const Boolean&) a - !!(const Boolean&) b;
			
			case Value_pair:
				{
					Expr& ax = *a.expr();
					Expr& bx = *b.expr();
					
					if ( ax.op != bx.op )
					{
						THROW( "can't compare atoms of different types" );
					}
					
					if ( const bool compare_left = (ax.op & 0xFF) != Op_scope )
					{
						if ( cmp_t cmp = compare_atoms( ax.left, bx.left ) )
						{
							return cmp;
						}
					}
					
					return compare_atoms( ax.right, bx.right );
				}
			
			default:
				INTERNAL_ERROR( "unsupported type in compare_atoms()" );
				break;
		}
		
		return false;
	}
	
	cmp_t compare( const Value& one, const Value& two )
	{
		if ( single_type_mismatch( one, two ) )
		{
			THROW( "mismatched types in comparison" );
		}
		
		list_iterator a( one );
		list_iterator b( two );
		
		while ( a  ||  b )
		{
			if ( cmp_t cmp = compare_atoms( a.use(), b.use() ) )
			{
				return cmp;
			}
		}
		
		return 0;
	}
	
}
