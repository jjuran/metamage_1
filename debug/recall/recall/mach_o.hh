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


namespace recall
{
	
	const char* FindSymbolName( return_address_mach_o addr );
	
	inline std::string GetSymbolString( const char* name )
	{
		return name;
	}
	
}

#endif

