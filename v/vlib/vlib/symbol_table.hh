/*
	symbol_table.hh
	---------------
*/

#ifndef VLIB_SYMBOLTABLE_HH
#define VLIB_SYMBOLTABLE_HH

// plus
#include "plus/string.hh"

// vlib
#include "vlib/symbol.hh"
#include "vlib/symbol_id.hh"


namespace vlib
{
	
	symbol_id locate_symbol( const plus::string& name );
	symbol_id create_symbol( const plus::string& name, symbol_type type );
	
}

#endif
