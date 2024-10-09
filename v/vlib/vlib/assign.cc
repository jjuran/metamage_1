/*
	assign.cc
	---------
*/

#include "vlib/assign.hh"

// debug
#include "debug/assert.hh"

// vlib
#include "vlib/equal.hh"
#include "vlib/targets.hh"
#include "vlib/throw.hh"
#include "vlib/tracker.hh"  // codependent:  tracker > symbol > assign
#include "vlib/type_info.hh"
#include "vlib/dispatch/dispatch.hh"
#include "vlib/dispatch/typing.hh"
#include "vlib/iterators/list_iterator.hh"
#include "vlib/types/table.hh"


namespace vlib
{
	
	Value as_assigned( const Value& type, const Value& v );
	
	static
	bool is_homogenous_array( const Value& type, const Value& array )
	{
		if ( is_empty_array( array ) )
		{
			return true;
		}
		
		Expr* expr = array.expr();
		
		if ( expr == 0  ||  expr->op != Op_array )  // NULL
		{
			return false;
		}
		
		list_iterator it( expr->right );
		
		while ( it )
		{
			if ( ! as_assigned( type, it.use() ) )
			{
				return false;
			}
		}
		
		return true;
	}
	
	static
	bool is_reference( const Value& type, Expr* vx )
	{
		if ( vx == 0  ||  vx->op != Op_unary_refer )  // NULL
		{
			return false;
		}
		
		Target target = make_target( vx->right );
		
		return as_assigned( type, *target.addr );
	}
	
	static
	bool is_mapping( Expr* tx, Expr* vx )
	{
		return      vx != 0  &&  vx->op == Op_mapping  // NULL
		        &&  as_assigned( tx->left,  vx->left  )
		        &&  as_assigned( tx->right, vx->right );
	}
	
	static
	bool is_table( Expr* tx, const Value& v )
	{
		return Table::test( v )  &&  equal( tx->left, v.expr()->left );
	}
	
	Value as_assigned( const Value& type, const Value& v )
	{
		if ( is_empty_list( type ) )
		{
			return is_empty_list( v ) ? v : NIL;
		}
		
		if ( const dispatch* methods = type.dispatch_methods() )
		{
			if ( const typing* typ = methods->type )
			{
				if ( constructor construct = typ->construct )
				{
					return construct( type, v );
				}
				
				if ( typechecker typecheck = typ->typecheck )
				{
					return typecheck( type, v ) ? v : NIL;
				}
			}
		}
		
		if ( Expr* expr = type.expr() )
		{
			if ( expr->op == Op_subscript )
			{
				return is_homogenous_array( expr->left, v ) ? v : NIL;
			}
			
			if ( expr->op == Op_unary_deref )
			{
				return is_reference( expr->right, v.expr() ) ? v : NIL;
			}
			
			if ( expr->op == Op_mapping )
			{
				return is_mapping( expr, v.expr() ) ? v : NIL;
			}
			
			if ( expr->op == Op_empower )
			{
				return is_table( expr, v ) ? v : NIL;
			}
			
			const Value& result = as_assigned( expr->left, v );
			
			if ( ! result == (expr->op == Op_union) )
			{
				return as_assigned( expr->right, v );
			}
			
			return result;
		}
		
		if ( type.type() != Value_base_type )
		{
			// TODO:  Support type lists.
			
			THROW( "`isa` right operand must be a type" );
		}
		
		return type.typeinfo().assign( v );
	}
	
	static
	Value as_coerced( const Value& type, const Value& v )
	{
		if ( is_empty_list( type ) )
		{
			return is_empty_list( v ) ? v : NIL;
		}
		
		if ( const dispatch* methods = type.dispatch_methods() )
		{
			if ( const typing* typ = methods->type )
			{
				if ( transformer transform = typ->transform )
				{
					return transform( type, v );
				}
				
				if ( typechecker typecheck = typ->typecheck )
				{
					return typecheck( type, v ) ? v : NIL;
				}
			}
		}
		
		if ( Expr* expr = type.expr() )
		{
			THROW( "coercion to expression types is undefined" );
		}
		
		ASSERT( type.type() == Value_base_type );
		
		const type_info& typeinfo = type.typeinfo();
		
		return (typeinfo.coerce ? typeinfo.coerce : typeinfo.assign)( v );
	}
	
	static inline
	void safe_overwrite( Value& dst, const Value& src )
	{
	#ifdef __RELIX__
		
		dst = src;
		
	#else
		
		gc_safe_overwrite( dst, src );
		
	#endif
	}
	
	void assign( const Target&  target,
	             const Value&   v,
	             bool           coercive )
	{
		Value& dst         = *target.addr;
		Value const& vtype = *target.type;
		
		if ( is_undefined( v )  &&  is_undefined( dst ) )
		{
			// Don't subject `let x (T) = ???` to type checks.
			
			return;
		}
		
		if ( vtype )
		{
			Value tmp = (coercive ? as_coerced : as_assigned)( vtype, v );
			
			if ( ! tmp )
			{
				THROW( "type mismatch in assignment" );
			}
			
			dst = tmp;
		}
		else
		{
			safe_overwrite( dst, v );
		}
	}
	
}
