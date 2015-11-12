/*
	symbol_table.cc
	---------------
*/

#include "vlib/symbol_table.hh"

// Standard C++
#include <list>

// vlib
#include "vlib/error.hh"


namespace vlib
{
	
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
		symbol_table.push_back( constant( "true",  Value( true  ) )  );
	}
	
	static symbol_table_init init;
	
}
