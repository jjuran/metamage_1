/*
	collectible.cc
	--------------
*/

#include "vlib/collectible.hh"

// vlib
#include "vlib/error.hh"
#include "vlib/symbol.hh"
#include "vlib/tracker.hh"
#include "vlib/types.hh"
#include "vlib/dispatch/dispatch.hh"
#include "vlib/dispatch/refs.hh"
#include "vlib/iterators/list_iterator.hh"
#include "vlib/types/any.hh"
#include "vlib/types/iterator.hh"
#include "vlib/types/record.hh"


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
		and `...` can be anything.  Code blocks can refer to symbols by
		closing over them, and a reference loop occurs if the code block is
		assigned to that symbol (or otherwise contained or referenced by its
		value).
		
		Reference objects can refer to themselves, but only if they allow
		referents of self-referential type.  (For example, (*...) allows
		self-ref, but (*int) does not, nor does (**int) -- and no amount of
		additional indirection will allow self-ref without ... or a closure.)
		Cyclical data structures are possible with typeless references, and
		references to closures have the usual issue, but the recursive type
		check covers these cases.
	*/
	
	static
	bool type_is_collectible( const Value& type )
	{
		if ( ! type )
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
			
			return + &typeinfo == &etc_vtype
			       | &typeinfo == &function_vtype
			       | &typeinfo == &iterator_vtype
			       | &typeinfo == &record_vtype;
		}
		
		if ( Expr* expr = type.expr() )
		{
			switch ( expr->op )
			{
				case Op_empower:
					/*
						This is a table.
						Assume the worst until we nail down the type details.
					*/
					return true;
				
				case Op_mapping:
				case Op_union:
					return type_is_collectible( expr->left  )  ||
					       type_is_collectible( expr->right );
				
				case Op_intersection:
					return type_is_collectible( expr->left  )  &&
					       type_is_collectible( expr->right );
				
				case Op_subscript:
					return type_is_collectible( expr->left );
				
				case Op_unary_deref:
					return type_is_collectible( expr->right );
				
				default:
					break;
			}
		}
		
		return true;
	}
	
	static inline
	bool value_has_custom_references( const Value& v )
	{
		if ( const dispatch* methods = v.dispatch_methods() )
		{
			return methods->ref;
		}
		
		return false;
	}
	
	static inline
	bool typed_value_is_collectible( const Value& v, const Value& type )
	{
		return ! v.is_cycle_free()  &&  type_is_collectible( type );
	}
	
	bool target_is_collectible( const Target& target )
	{
		const Value& v = *target.addr;
		
		if ( value_has_custom_references( v ) )
		{
			return true;
		}
		
		return typed_value_is_collectible( *target.addr, *target.type );
	}
	
	static inline
	bool symbol_is_collectible( const Symbol& symbol )
	{
		return type_is_collectible( symbol.vtype() );
	}
	
	void track_symbol_if_collectible( const Value& symbol )
	{
		if ( symbol_is_collectible( *symbol.sym() ) )
		{
			track_symbol( symbol );
		}
	}
	
	bool symbol_list_with_values_is_collectible( const Value& symlist )
	{
		list_iterator it( symlist );
		
		while ( const Symbol* sym = it.use().sym() )
		{
			if ( sym->mark() )
			{
				return true;
			}
			
			const Value& v = sym->get();
			
			if ( v.is_cycle_free() )
			{
				continue;
			}
			
			if ( value_has_custom_references( v ) )
			{
				return true;
			}
			
			if ( v.expr() != 0  &&  symbol_is_collectible( *sym ) )
			{
				return true;
			}
		}
		
		return false;
	}
	
}
