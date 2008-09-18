/*	==================
 *	TracebackTables.hh
 *	==================
 */

#ifndef BACKTRACE_TRACEBACKTABLES_HH
#define BACKTRACE_TRACEBACKTABLES_HH

#if PRAGMA_ONCE
#pragma once
#endif

// Standard C++
#include <string>

// Backtrace
#include "Backtrace/StackCrawl.hh"


namespace Backtrace
{
	
	struct TracebackTable
	{
		unsigned long   zero;
		unsigned long   unknown1;
		unsigned long   unknown2;
		unsigned long   codeSize;
		unsigned short  nameSize;
		const char      name[2];
	};
	
	typedef const TracebackTable* TracebackTablePtr;
	
	TracebackTablePtr FindSymbolName( ReturnAddrWithTraceback addr );
	
	inline std::string GetSymbolString( TracebackTablePtr table )
	{
		return std::string( table->name, table->nameSize );
	}
	
}

#endif

