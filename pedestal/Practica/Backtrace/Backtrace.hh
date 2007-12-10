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
	
	struct CallInfo
	{
		const void*  itsReturnAddr;
		const char*  itsArch;
		std::string  itsUnmangledName;
	};
	
	CallInfo GetCallInfoFromReturnAddress( const ReturnAddress& call );
	
	std::string MakeReportFromCallChain( std::vector< CallInfo >::const_iterator  begin,
	                                     std::vector< CallInfo >::const_iterator  end );
	
	std::string MakeReportFromStackCrawl( std::vector< ReturnAddress >::const_iterator  begin,
	                                      std::vector< ReturnAddress >::const_iterator  end );
	
	class DebuggingContext
	{
		private:
			std::vector< ReturnAddress > itsStackCrawl;
		
		public:
			DebuggingContext();
			
			const std::vector< ReturnAddress >& GetStackCrawl() const  { return itsStackCrawl; }
	};
	
}

#endif

