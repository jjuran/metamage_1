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
#include "vlib/symbol_id.hh"


namespace vlib
{
	
	class symbol_table
	{
		private:
			std::list< Value > its_symbols;
		
		public:
			void define_constant( const char* name, const Value& v );
			
			symbol_id locate( const plus::string& name ) const;
			symbol_id create( const plus::string& name, symbol_type type );
	};
	
	symbol_id locate_symbol( const plus::string& name );
	symbol_id create_symbol( const plus::string& name, symbol_type type );
	
}

#endif
