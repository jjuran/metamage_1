/*	==================
 *	macsbug_symbols.hh
 *	==================
 */

#ifndef RECALL_MACSBUGSYMBOLS_HH
#define RECALL_MACSBUGSYMBOLS_HH

// Standard C++
#include <string>

// Recall
#include "recall/stack_crawl.hh"


namespace recall
{
	
	struct MacsbugSymbol;
	
	typedef const struct MacsbugSymbol* MacsbugSymbolPtr;
	
	
	MacsbugSymbolPtr FindSymbolName( return_address_68k addr );
	
	std::string GetSymbolString( MacsbugSymbolPtr symbol );
	
}

#endif

