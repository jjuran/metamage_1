/*
	equal.cc
	--------
*/

#include "vlib/equal.hh"

// plus
#include "plus/integer.hh"

// vlib
#include "vlib/error.hh"
#include "vlib/throw.hh"
#include "vlib/iterators/list_iterator.hh"


namespace vlib
{
	
	static
	bool equal_atoms( const Value& a, const Value& b )
	{
		if ( a.type() != b.type() )
		{
			return false;
		}
		
		switch ( a.type() )
		{
			case Value_nothing:
			case Value_dummy_operand:
			case Value_empty_list:
			case Value_empty_array:
				return true;
			
			case Value_symbol:
				return a.sym() == b.sym();
			
			case Value_boolean:
				return a.boolean() == b.boolean();
			
			case Value_byte:
			case Value_number:
				return a.number() == b.number();
			
			case Value_mb32:
			case Value_vector:
			case Value_string:
				return a.string() == b.string();
			
			case Value_function:
				return &a.proc() == &b.proc();
			
			case Value_base_type:
				return &a.typeinfo() == &b.typeinfo();
			
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
	
	static
	bool single_type_mismatch( const Value& one, const Value& two )
	{
		if ( ! is_single( one )  ||  ! is_single( two ) )
		{
			return false;
		}
		
		if ( one.type() != two.type() )
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
