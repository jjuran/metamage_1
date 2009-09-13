/*	========
 *	MachO.hh
 *	========
 */

#ifndef BACKTRACE_MACHO_HH
#define BACKTRACE_MACHO_HH

// Standard C++
#include <string>

// Recall
#include "recall/stack_crawl.hh"


namespace Backtrace
{
	
	const char* FindSymbolName( ReturnAddrMachO addr );
	
	inline std::string GetSymbolString( const char* name )
	{
		return name;
	}
	
}

#endif

