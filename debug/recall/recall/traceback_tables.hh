/*	==================
 *	TracebackTables.hh
 *	==================
 */

#ifndef BACKTRACE_TRACEBACKTABLES_HH
#define BACKTRACE_TRACEBACKTABLES_HH

// Standard C++
#include <string>

// Recall
#include "recall/stack_crawl.hh"


namespace recall
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

