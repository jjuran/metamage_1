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
	
	static std::list< Symbol > global_symbol_table;
	
	
	symbol_id locate_symbol( const plus::string& name )
	{
		typedef std::list< Symbol >::iterator Iter;
		
		Iter begin = global_symbol_table.begin();
		Iter it    = global_symbol_table.end();
		
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
		
		global_symbol_table.push_back( Symbol( type, name ) );
		
		return &global_symbol_table.back();
	}
	
}
