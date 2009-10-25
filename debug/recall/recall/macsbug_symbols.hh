/*	==================
 *	macsbug_symbols.hh
 *	==================
 */

#ifndef RECALL_MACSBUGSYMBOLS_HH
#define RECALL_MACSBUGSYMBOLS_HH

// Standard C++
#include <string>

// Recall
#include "recall/return_address.hh"


namespace recall
{
	
	struct macsbug_symbol;
	
	const macsbug_symbol* find_symbol_name( return_address_68k addr );
	
	std::string get_symbol_string( const macsbug_symbol* symbol );
	
}

#endif

