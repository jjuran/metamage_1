/*
	scope.cc
	--------
*/

#include "vlib/scope.hh"

// debug
#include "debug/assert.hh"

// vlib
#include "vlib/error.hh"
#include "vlib/symdesc.hh"


namespace vlib
{
	
	const Value& lexical_scope::resolve( const plus::string& name, int depth )
	{
		if ( const Value& sym = locate_symbol( name ) )
		{
			return sym;
		}
		
		if ( const Value& sym = locate_symbol( its_symbol_cache, name ) )
		{
			return sym;
		}
		
		lexical_scope* scope = this;
		
		do
		{
			int offset = scope->its_symbols.offset( name );
			
			if ( offset < 0 )
			{
				++depth;
				scope = scope->its_parent;
				continue;
			}
			
			const Value metasymbol = make_metasymbol( name, depth, offset );
			
			its_symbol_cache.push_back( metasymbol );
			
			return its_symbol_cache.back();
		}
		while ( scope != NULL );
		
		return nothing;
	}
	
	const Value& lexical_scope::declare( const plus::string&  name,
	                                     symbol_type          type )
	{
		if ( locate_symbol( name ).type() )
		{
			SYMBOL_ERROR( "keyword override attempt" );
		}
		
		return its_symbols.create( name, type );
	}
	
	void lexical_scope_box::pop()
	{
		ASSERT( its_lexical_scope != NULL );
		
		if ( its_lexical_scope != NULL )
		{
			lexical_scope* popped = its_lexical_scope;
			
			its_lexical_scope = its_lexical_scope->parent();
			
			delete popped;
		}
	}
	
}
