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
		Value constant( Symbol_const, name );
		
		constant.sym()->assign( v );
		
		its_symbols.push_back( constant );
	}
	
	static
	const Value* find_symbol( const Symbols& symbols, const plus::string& name )
	{
		typedef Symbols::const_iterator Iter;
		
		Iter begin = symbols.begin();
		Iter it    = symbols.end();
		
		while ( it != begin )
		{
			const Value& v = *--it;
			
			Symbol* sym = v.sym();
			
			if ( name == sym->name() )
			{
				return &v;
			}
		}
		
		return NULL;
	}
	
	const Value& locate_symbol( const Symbols& syms, const plus::string& name )
	{
		if ( const Value* it = find_symbol( syms, name ) )
		{
			return *it;
		}
		
		return nothing;
	}
	
	const Value& symbol_table::create( const plus::string& name, symbol_type type )
	{
		if ( const Value* it = find_symbol( its_symbols, name ) )
		{
			const Value& sym = *it;
			
			Symbol& var = *sym;
			
			if ( type == Symbol_const  &&  var.is_var() )
			{
				var.constify();
				
				return sym;
			}
			
			SYMBOL_ERROR( "duplicate symbol" );
		}
		
		its_symbols.push_back( Value( type, name ) );
		
		return its_symbols.back();
	}
	
	const Value& locate_symbol( const plus::string& name )
	{
		return global_symbol_table.locate( name );
	}
	
	const Value& create_symbol( const plus::string& name, symbol_type type )
	{
		return global_symbol_table.create( name, type );
	}
	
}
