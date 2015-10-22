/*
	symbol_table.cc
	---------------
*/

#include "vlib/symbol_table.hh"

// Standard C++
#include <list>

// vlib
#include "vlib/error.hh"
#include "vlib/types.hh"


namespace vlib
{
	
	void Symbol::denote( const Value& vtype )
	{
		if ( vtype.type() != Value_base_type )
		{
			SYNTAX_ERROR( "type annotation not a single type" );
		}
		
		if ( is_defined() )
		{
			SYNTAX_ERROR( "type annotation of defined symbol" );
		}
	}
	
	void Symbol::assign( const Value& v )
	{
		if ( is_const()  &&  is_defined() )
		{
			SYMBOL_ERROR( "reassignment of constant" );
		}
		
		its_value = v;
	}
	
	Value& Symbol::deref()
	{
		if ( is_const() )
		{
			SYMBOL_ERROR( "modification of constant" );
		}
		
		return its_value;
	}
	
	static std::list< Symbol > symbol_table;
	
	
	symbol_id locate_symbol( const plus::string& name )
	{
		typedef std::list< Symbol >::iterator Iter;
		
		Iter begin = symbol_table.begin();
		Iter it    = symbol_table.end();
		
		while ( --it != begin )
		{
			if ( name == it->name() )
			{
				return &*it;
			}
		}
		
		return NULL;
	}
	
	symbol_id create_symbol( const plus::string& name, symbol_type type )
	{
		if ( symbol_id sym = locate_symbol( name ) )
		{
			Symbol& var = *sym;
			
			if ( type == Symbol_const  &&  var.is_var() )
			{
				var.constify();
				
				return sym;
			}
			
			SYMBOL_ERROR( "duplicate symbol" );
		}
		
		symbol_table.push_back( Symbol( type, name ) );
		
		return &symbol_table.back();
	}
	
	
	static inline
	Symbol constant( const char* name, const Value& value )
	{
		return Symbol( Symbol_const, name, value );
	}
	
	struct symbol_table_init
	{
		symbol_table_init();
	};
	
	symbol_table_init::symbol_table_init()
	{
		symbol_table.push_back( constant( "", Value_undefined ) );
		
		symbol_table.push_back( constant( "false", Value( false ) ) );
		symbol_table.push_back( constant( "true",  Value( true  ) ) );
		
		symbol_table.push_back( constant( "boolean",  boolean_vtype  ) );
		symbol_table.push_back( constant( "function", function_vtype ) );
		symbol_table.push_back( constant( "integer",  integer_vtype  ) );
		symbol_table.push_back( constant( "string",   string_vtype   ) );
		symbol_table.push_back( constant( "type",     type_vtype     ) );
	}
	
	static symbol_table_init init;
	
}
