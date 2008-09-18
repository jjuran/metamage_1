/*	========
 *	MachO.hh
 *	========
 */

#ifndef BACKTRACE_MACHO_HH
#define BACKTRACE_MACHO_HH

#if PRAGMA_ONCE
#pragma once
#endif

// Standard C++
#include <string>

// Backtrace
#include "Backtrace/StackCrawl.hh"


namespace Backtrace
{
	
	const char* FindSymbolName( ReturnAddrMachO addr );
	
	inline std::string GetSymbolString( const char* name )
	{
		return name;
	}
	
}

#endif

