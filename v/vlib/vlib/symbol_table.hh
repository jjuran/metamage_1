/*
	symbol_table.hh
	---------------
*/

#ifndef VLIB_SYMBOLTABLE_HH
#define VLIB_SYMBOLTABLE_HH

// Standard C++
#include <vector>

// plus
#include "plus/string.hh"

// vlib
#include "vlib/symbol.hh"


namespace vlib
{
	
	typedef std::vector< Value > Symbols;
	
	const Value& locate_symbol( const Symbols& syms, const plus::string& name );
	
	void forget_symbol( Symbols& syms, const plus::string& name );
	
	class symbol_table
	{
		private:
			Symbols its_symbols;
		
		public:
			void define_constant( const char* name, const Value& v );
			
			int offset( const plus::string& name ) const;
			
			Value list() const;
			
			const Value& locate( const plus::string& name ) const
			{
				return locate_symbol( its_symbols, name );
			}
			
			const Value& create( const plus::string& name, symbol_type type );
			
			const Value& operator[]( std::size_t i ) const
			{
				return its_symbols[ i ];
			}
	};
	
	const Value& locate_keyword( const plus::string& name );
	const Value& create_keyword( const plus::string& name );
	
}

#endif
