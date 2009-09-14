/*	===================
 *	traceback_tables.hh
 *	===================
 */

#ifndef RECALL_TRACEBACKTABLES_HH
#define RECALL_TRACEBACKTABLES_HH

// Standard C++
#include <string>

// Recall
#include "recall/stack_crawl.hh"


namespace recall
{
	
	struct TracebackTable;
	
	typedef const TracebackTable* TracebackTablePtr;
	
	TracebackTablePtr FindSymbolName( return_address_traceback addr );
	
	std::string GetSymbolString( TracebackTablePtr table );
	
}

#endif

