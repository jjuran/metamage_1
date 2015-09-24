/*
	symbol_table.hh
	---------------
*/

#ifndef VC_SYMBOLTABLE_HH
#define VC_SYMBOLTABLE_HH

// plus
#include "plus/string.hh"

// vc
#include "vc/symbol_id.hh"
#include "vc/value.hh"


namespace vc
{
	
	symbol_id locate_symbol( const plus::string& name );
	
	const Value& lookup_symbol( symbol_id id );
	
}

#endif
