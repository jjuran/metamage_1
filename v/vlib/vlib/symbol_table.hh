/*
	symbol_table.hh
	---------------
*/

#ifndef VLIB_SYMBOLTABLE_HH
#define VLIB_SYMBOLTABLE_HH

// Standard C++
#include <list>

// plus
#include "plus/string.hh"

// vlib
#include "vlib/symbol.hh"


namespace vlib
{
	
	typedef std::list< Value > Symbols;
	
	class symbol_table
	{
		private:
			Symbols its_symbols;
		
		public:
			void define_constant( const char* name, const Value& v );
			
			const Value& locate( const plus::string& name ) const;
			const Value& create( const plus::string& name, symbol_type type );
	};
	
	const Value& locate_symbol( const plus::string& name );
	const Value& create_symbol( const plus::string& name, symbol_type type );
	
}

#endif
