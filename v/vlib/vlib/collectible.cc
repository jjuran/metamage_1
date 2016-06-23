/*
	collectible.cc
	--------------
*/

#include "vlib/collectible.hh"

// vlib
#include "vlib/error.hh"
#include "vlib/symbol.hh"
#include "vlib/types.hh"
#include "vlib/types/any.hh"


namespace vlib
{
	
	/*
		In order to provide timely destruction of volatile resources, we
		need to run a garbage collection pass to remove cyclical references
		(that might be referring to such an object) whenever a reference that
		might in turn be harboring the cyclical references is discarded.
		
		In other words, we don't need to check for volatile objects -- the
		usual reference counting mechanism is sufficient to destroy them as
		necessary -- unless a reference loop is involved.  It's the loops
		that we need to check for.
		
		A reference is discarded either (a) when it's the left operand of
		an assignment, or (b) when it's a local symbol in a scope that has
		just been exited.
		
		Assignment refers to duplication, approximation, and move only.
		Update assignment doesn't apply in principle, because while it
		modifies a value, it doesn't discard a reference to it.  In practice,
		it does normal assignment instead of updates, but the only operators
		currently defined only work on integers.  The push operator `<--`
		reuses the existing items in the array on the left, so nothing is
		discarded.  Swap (`<->`) obviously doesn't discard anything.
		
		For the purpose of collectibility, whether a scope's local symbols
		include `_` depends on how it was invoked.  If it was called, then
		`_` is a fresh clone that will be destroyed on return, so it needs
		to be considered.  If the scope was merely entered, then `_` is not
		a local copy and will persist afterward, so it can be skipped.
		
		Not all discarded values require garbage collection -- only those
		that can refer to other values need to be considered.  First, we
		check the value itself.  Only pairs (a.k.a. cons cells) refer to
		other values (via the Expr structure), so a value without an Expr
		requires no further action.  Since we can determine this in constant
		time (and a very short time, at that), we check this first.
		
		Next we check the type.  If the symbol has a type annotation, we can
		be sure that any value it stores conforms to that type.  For example,
		argv has type `c_str[]`.  Therefore, every value it references is
		either a string or a pair, and pairs can't be constructed in cycles
		without other types.  This check is of linear complexity, but on the
		depth of the type -- e.g. for `int[] | str[]`, type_is_collectible()
		is called five times.  Array element counts are irrelevant -- for
		`byte[ 256 ]`, n is just 2.
		
		For type expressions, one or both component types are checked as
		needed.  Of the base types, `function` can refer to a code block,
		and `...` can be anything.  Code blocks are the only values that
		can refer to symbols, by closing over them.  Since cyclical data
		structures are not allowed, reference loops only occur when a block
		closes over the symbol to which it's assigned, or another symbol that
		references it indirectly.
	*/
	
	static
	bool type_is_collectible( const Value& type )
	{
		if ( ! type.type() )
		{
			return true;
		}
		
		if ( is_empty_list( type ) )
		{
			return false;
		}
		
		if ( type.type() == Value_base_type )
		{
			const type_info& typeinfo = type.typeinfo();
			
			return &typeinfo == &etc_vtype  ||  &typeinfo == &function_vtype;
		}
		
		if ( Expr* expr = type.expr() )
		{
			switch ( expr->op )
			{
				case Op_mapping:
				case Op_union:
					return type_is_collectible( expr->left  )  ||
					       type_is_collectible( expr->right );
				
				case Op_intersection:
					return type_is_collectible( expr->left  )  &&
					       type_is_collectible( expr->right );
				
				case Op_subscript:
					return type_is_collectible( expr->left );
				
				default:
					break;
			}
		}
		
		INTERNAL_ERROR( "unexpected type in type_is_collectible()" );
		return true;
	}
	
	bool symbol_is_collectible( const Symbol& symbol, const Value& v )
	{
		return v.expr() != 0  &&  type_is_collectible( symbol.vtype() );
	}
	
}
