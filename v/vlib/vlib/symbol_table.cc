/*
	symbol_table.cc
	---------------
*/

#include "vlib/symbol_table.hh"

// vlib
#include "vlib/error.hh"


namespace vlib
{
	
	static symbol_table global_symbol_table;
	
	
	void symbol_table::define_constant( const char* name, const Value& v )
	{
		its_symbols.push_back( Symbol( Symbol_const, name, v ) );
	}
	
	symbol_id symbol_table::locate( const plus::string& name )
	{
		typedef std::list< Symbol >::iterator Iter;
		
		Iter begin = its_symbols.begin();
		Iter it    = its_symbols.end();
		
		while ( it != begin )
		{
			Symbol* sym = &*--it;
			
			if ( name == sym->name() )
			{
				return sym;
			}
		}
		
		return NULL;
	}
	
	symbol_id symbol_table::create( const plus::string& name, symbol_type type )
	{
		if ( symbol_id sym = locate( name ) )
		{
			Symbol& var = *sym;
			
			if ( type == Symbol_const  &&  var.is_var() )
			{
				var.constify();
				
				return sym;
			}
			
			SYMBOL_ERROR( "duplicate symbol" );
		}
		
		its_symbols.push_back( Symbol( type, name ) );
		
		return &its_symbols.back();
	}
	
	symbol_id locate_symbol( const plus::string& name )
	{
		return global_symbol_table.locate( name );
	}
	
	symbol_id create_symbol( const plus::string& name, symbol_type type )
	{
		return global_symbol_table.create( name, type );
	}
	
}
