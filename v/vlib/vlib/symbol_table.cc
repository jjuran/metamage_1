/*
	symbol_table.cc
	---------------
*/

#include "vlib/symbol_table.hh"

// vlib
#include "vlib/throw.hh"
#include "vlib/types/term.hh"


namespace vlib
{
	
	static symbol_table keyword_symbol_table;
	
	
	void symbol_table::define_constant( const char* name, const Value& v )
	{
		Constant constant( name );
		
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
	
	int symbol_table::offset( const plus::string& name ) const
	{
		if ( const Value* it = find_symbol( its_symbols, name ) )
		{
			return it - &*its_symbols.begin();
		}
		
		return -1;
	}
	
	const Value& locate_symbol( const Symbols& syms, const plus::string& name )
	{
		if ( const Value* it = find_symbol( syms, name ) )
		{
			return *it;
		}
		
		return NIL;
	}
	
	void forget_symbol( Symbols& syms, const plus::string& name )
	{
		if ( const Value* it = find_symbol( syms, name ) )
		{
			int offset = it - &*syms.begin();
			
			Value& target = syms[ offset ];
			
			target = Term( Symbol_const, plus::string::null );
		}
	}
	
	const Value& symbol_table::create( const plus::string& name, symbol_type type )
	{
		if ( const Value* it = find_symbol( its_symbols, name ) )
		{
			const Value& symbol = *it;
			
			Symbol& var = *symbol.sym();
			
			if ( type == Symbol_const  &&  var.is_var() )
			{
				var.constify();
				
				return symbol;
			}
			
			THROW( "duplicate symbol" );
		}
		
		its_symbols.push_back( Term( type, name ) );
		
		return its_symbols.back();
	}
	
	Value symbol_table::list() const
	{
		if ( its_symbols.empty() )
		{
			return Value_empty_list;
		}
		
		typedef Symbols::const_iterator Iter;
		
		Iter begin = its_symbols.begin();
		Iter it    = its_symbols.end();
		
		Value result = *--it;
		
		while ( it != begin )
		{
			result = Value( *--it, result );
		}
		
		return result;
	}
	
	const Value& locate_keyword( const plus::string& name )
	{
		return keyword_symbol_table.locate( name );
	}
	
	const Value& create_keyword( const plus::string& name )
	{
		return keyword_symbol_table.create( name, Symbol_const );
	}
	
}
