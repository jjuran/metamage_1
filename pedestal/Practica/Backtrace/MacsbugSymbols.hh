/*	=================
 *	MacsbugSymbols.hh
 *	=================
 */

#ifndef BACKTRACE_MACSBUGSYMBOLS_HH
#define BACKTRACE_MACSBUGSYMBOLS_HH

#if PRAGMA_ONCE
#pragma once
#endif

// Standard C++
#include <string>

// Backtrace
#include "Backtrace/StackCrawl.hh"


namespace Backtrace
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

