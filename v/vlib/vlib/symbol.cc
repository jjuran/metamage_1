/*
	symbol.cc
	---------
*/

#include "vlib/symbol.hh"

// vlib
#include "vlib/error.hh"
#include "vlib/type_info.hh"


namespace vlib
{
	
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
			const type_info& type = its_vtype.typeinfo();
			
			coerce_proc coerce = coercive  &&  type.coerce ? type.coerce
			                                               : type.assign;
			
			its_value = coerce( v );
			
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
