/*	=================
 *	MacsbugSymbols.hh
 *	=================
 */

#ifndef BACKTRACE_MACSBUGSYMBOLS_HH
#define BACKTRACE_MACSBUGSYMBOLS_HH

// Standard C++
#include <string>

// Recall
#include "recall/stack_crawl.hh"


namespace recall
{
	
	struct MacsbugSymbol
	{
		unsigned char bytes[1];
	};
	
	typedef const struct MacsbugSymbol* MacsbugSymbolPtr;
	
	
	MacsbugSymbolPtr FindSymbolName( ReturnAddr68K addr );
	
	std::string GetSymbolString( MacsbugSymbolPtr symbol );
	
}

#endif

