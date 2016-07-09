/*
	symbol.cc
	---------
*/

#include "vlib/symbol.hh"

// debug
#include "debug/assert.hh"

// vlib
#include "vlib/throw.hh"
#include "vlib/type_info.hh"


namespace vlib
{
	
	Value as_assigned( const Value& type, const Value& v );
	
	static
	bool is_homogenous_array( const Value& type, const Value& array )
	{
		Expr* expr = array.expr();
		
		if ( expr == 0  ||  expr->op != Op_array )  // NULL
		{
			return false;
		}
		
		const Value* list = &expr->right;
		
		while ( ! is_empty( *list ) )
		{
			const Value& next = first( *list );
			
			if ( ! as_assigned( type, next ).type() )
			{
				return false;
			}
			
			list = &rest( *list );
		}
		
		return true;
	}
	
	static
	bool is_mapping( Expr* tx, Expr* vx )
	{
		return      vx != 0  &&  vx->op == Op_mapping  // NULL
		        &&  as_assigned( tx->left,  vx->left  ).type()
		        &&  as_assigned( tx->right, vx->right ).type();
	}
	
	Value as_assigned( const Value& type, const Value& v )
	{
		if ( is_empty( type ) )
		{
			return is_empty( v ) ? v : Value_nothing;
		}
		
		if ( Expr* expr = type.expr() )
		{
			if ( expr->op == Op_subscript )
			{
				return is_homogenous_array( expr->left, v ) ? v : nothing;
			}
			
			if ( expr->op == Op_mapping )
			{
				return is_mapping( expr, v.expr() ) ? v : nothing;
			}
			
			Value result = as_assigned( expr->left, v );
			
			if ( (! result.type()) == (expr->op == Op_union) )
			{
				result = as_assigned( expr->right, v );
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
		if ( is_empty( type ) )
		{
			return is_empty( v ) ? v : Value_nothing;
		}
		
		if ( Expr* expr = type.expr() )
		{
			THROW( "coercion to expression types is undefined" );
		}
		
		ASSERT( type.type() == Value_base_type );
		
		const type_info& typeinfo = type.typeinfo();
		
		return (typeinfo.coerce ? typeinfo.coerce : typeinfo.assign)( v );
	}
	
	void Symbol::denote( const Value& vtype )
	{
		if ( ! is_type( vtype ) )
		{
			THROW( "type annotation not a type" );
		}
		
		if ( its_vtype.type() != Value_nothing )
		{
			THROW( "reannotation of type-annotated symbol" );
		}
		
		if ( is_defined() )
		{
			THROW( "type annotation of defined symbol" );
		}
		
		its_vtype = vtype;
	}
	
	void Symbol::assign( const Value& v, bool coercive )
	{
		if ( is_immutable() )
		{
			THROW( "reassignment of constant" );
		}
		
		if ( its_vtype.type() )
		{
			its_value = (coercive ? as_coerced : as_assigned)( its_vtype, v );
			
			if ( ! its_value.type() )
			{
				THROW( "type mismatch in assignment" );
			}
		}
		else
		{
			its_value = v;
		}
	}
	
	Value& Symbol::deref()
	{
		if ( is_const() )
		{
			THROW( "modification of constant" );
		}
		
		its_value.unshare();
		
		return its_value;
	}
	
	Value Symbol::clone() const
	{
		Value result( its_type, name() );
		
		Symbol* sym = result.sym();
		
		if ( its_vtype.type() )
		{
			sym->denote( its_vtype );
		}
		
		sym->assign( its_value );
		
		return result;
	}
	
}
