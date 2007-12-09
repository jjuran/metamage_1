/*	============
 *	Backtrace.hh
 *	============
 */

#ifndef BACKTRACE_BACKTRACE_HH
#define BACKTRACE_BACKTRACE_HH

// Standard C++
#include <string>
#include <vector>

// Backtrace
#include "Backtrace/StackCrawl.hh"


namespace Backtrace
{
	
	struct TraceRecord
	{
		const void* itsReturnAddr;
		const char* itsArch;
		std::string itsUnmangledName;
	};
	
	TraceRecord TraceCall( const CallRecord& call );
	
	std::string GetBacktrace( const std::vector< CallRecord >& stackCrawl );
	
	struct DebuggingContext
	{
		std::vector< CallRecord > itsStackCrawl;
		
		DebuggingContext();
		
		std::string GetText() const  { return GetBacktrace( itsStackCrawl ); }
		
		void Show() const;
	};
	
}

#endif

