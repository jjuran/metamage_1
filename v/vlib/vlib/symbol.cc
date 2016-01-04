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
	
	static
	Value as_assigned( const Value& type, const Value& v )
	{
		ASSERT( type.type() == Value_base_type );
		
		return type.typeinfo().assign( v );
	}
	
	static
	Value as_coerced( const Value& type, const Value& v )
	{
		ASSERT( type.type() == Value_base_type );
		
		const type_info& typeinfo = type.typeinfo();
		
		return (typeinfo.coerce ? typeinfo.coerce : typeinfo.assign)( v );
	}
	
	void Symbol::denote( const Value& vtype )
	{
		if ( vtype.type() != Value_base_type )
		{
			SYNTAX_ERROR( "type annotation not a single type" );
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
