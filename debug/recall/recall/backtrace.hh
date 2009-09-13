/*	============
 *	Backtrace.hh
 *	============
 */

#ifndef BACKTRACE_BACKTRACE_HH
#define BACKTRACE_BACKTRACE_HH

// Standard C++
#include <string>
#include <vector>

// Recall
#include "recall/stack_crawl.hh"


namespace recall
{
	
	struct CallInfo
	{
		const void*  itsFramePtr;
		const void*  itsReturnAddr;
		const char*  itsArch;
		std::string  itsUnmangledName;
	};
	
	CallInfo GetCallInfoFromReturnAddress( const FrameData& call );
	
	std::string MakeReportFromCallChain( std::vector< CallInfo >::const_iterator  begin,
	                                     std::vector< CallInfo >::const_iterator  end );
	
	std::string MakeReportFromStackCrawl( std::vector< FrameData >::const_iterator  begin,
	                                      std::vector< FrameData >::const_iterator  end );
	
	class DebuggingContext
	{
		private:
			std::vector< FrameData > itsStackCrawl;
		
		public:
			DebuggingContext();
			
			const std::vector< FrameData >& GetStackCrawl() const  { return itsStackCrawl; }
	};
	
	void SetStackBottomLimit( const void* limit );
	
}

#endif

