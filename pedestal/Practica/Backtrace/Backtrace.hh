/*	============
 *	Backtrace.hh
 *	============
 */

#ifndef BACKTRACE_BACKTRACE_HH
#define BACKTRACE_BACKTRACE_HH

// Standard C++
#include <vector>

// Backtrace
#include "Backtrace/StackCrawl.hh"


namespace Backtrace
{
	
	struct DebuggingContext
	{
		std::vector< CallRecord > itsStackCrawl;
		
		DebuggingContext();
		
		void Show() const;
	};
	
}

#endif

