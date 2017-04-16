/*
	scope.cc
	--------
*/

#include "vlib/scope.hh"

// debug
#include "debug/assert.hh"

// vlib
#include "vlib/symdesc.hh"
#include "vlib/throw.hh"


namespace vlib
{
	
	const Value& lexical_scope::resolve( const plus::string& name, int depth )
	{
		if ( const Value& sym = locate_keyword( name ) )
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
			
			const Value& symbol = scope->its_symbols[ offset ];
			
			const bool immutable = symbol.sym()->is_immutable();
			
			
			const Value s = immutable ? symbol
			                          : make_metasymbol( name, depth, offset );
			
			its_symbol_cache.push_back( s );
			
			return its_symbol_cache.back();
		}
		while ( scope != NULL );
		
		return nothing;
	}
	
	const Value& lexical_scope::declare( const plus::string&  name,
	                                     symbol_type          type )
	{
		if ( locate_keyword( name ).type() )
		{
			THROW( "keyword override attempt" );
		}
		
		return its_symbols.create( name, type );
	}
	
	const Value& lexical_scope::immortalize_constant( int i, const Value& v )
	{
		const Value& symbol = its_symbols[ i ];
		
		symbol.sym()->assign( v );
		
		return symbol;
	}
	
	void lexical_scope_box::pop()
	{
		ASSERT( its_lexical_scope != its_bottom_scope );
		
		if ( its_lexical_scope != its_bottom_scope )
		{
			lexical_scope* popped = its_lexical_scope;
			
			its_lexical_scope = its_lexical_scope->parent();
			
			delete popped;
		}
	}
	
}
