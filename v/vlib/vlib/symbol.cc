/*
	symbol.cc
	---------
*/

#include "vlib/symbol.hh"

// debug
#include "debug/assert.hh"

// vlib
#include "vlib/error.hh"
#include "vlib/type_info.hh"


namespace vlib
{
	
	Value as_assigned( const Value& type, const Value& v );
	
	static
	bool is_homogenous_array( const Value& type, const Value& array )
	{
		Expr* expr = array.expr();
		
		if ( expr == NULL  ||  expr->op != Op_array )
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
	
	Value as_assigned( const Value& type, const Value& v )
	{
		if ( Expr* expr = type.expr() )
		{
			if ( expr->op == Op_subscript )
			{
				return is_homogenous_array( expr->left, v ) ? v : nothing;
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
			
			TYPE_ERROR( "`isa` right operand must be a type" );
		}
		
		return type.typeinfo().assign( v );
	}
	
	static
	Value as_coerced( const Value& type, const Value& v )
	{
		if ( Expr* expr = type.expr() )
		{
			TYPE_ERROR( "coercion to expression types is undefined" );
		}
		
		ASSERT( type.type() == Value_base_type );
		
		const type_info& typeinfo = type.typeinfo();
		
		return (typeinfo.coerce ? typeinfo.coerce : typeinfo.assign)( v );
	}
	
	void Symbol::denote( const Value& vtype )
	{
		if ( ! is_type( vtype ) )
		{
			SYNTAX_ERROR( "type annotation not a type" );
		}
		
		if ( its_vtype.type() != Value_nothing )
		{
			SYNTAX_ERROR( "reannotation of type-annotated symbol" );
		}
		
		if ( is_defined() )
		{
			SYNTAX_ERROR( "type annotation of defined symbol" );
		}
		
		its_vtype = vtype;
	}
	
	void Symbol::assign( const Value& v, bool coercive )
	{
		if ( is_immutable() )
		{
			SYMBOL_ERROR( "reassignment of constant" );
		}
		
		if ( its_vtype.type() )
		{
			its_value = (coercive ? as_coerced : as_assigned)( its_vtype, v );
			
			if ( ! its_value.type() )
			{
				TYPE_ERROR( "type mismatch in assignment" );
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
			SYMBOL_ERROR( "modification of constant" );
		}
		
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
