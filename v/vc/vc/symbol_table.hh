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
	
	enum symbol_type
	{
		Symbol_const,
		Symbol_var,
	};
	
	symbol_id locate_symbol( const plus::string& name );
	symbol_id create_symbol( const plus::string& name, symbol_type type );
	
	void assign_symbol( symbol_id id, const Value& value );
	
	const Value& lookup_symbol( symbol_id id );
	
	Value& modify_symbol( symbol_id id );
	
}

#endif
