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
	
	std::string MakeReportFromStackCrawl( std::vector< frame_data >::const_iterator  begin,
	                                      std::vector< frame_data >::const_iterator  end );
	
	class DebuggingContext
	{
		private:
			std::vector< frame_data > itsStackCrawl;
		
		public:
			DebuggingContext();
			
			const std::vector< frame_data >& get_stack_crawl() const  { return itsStackCrawl; }
	};
	
	void SetStackBottomLimit( const void* limit );
	
}

#endif

