/*	============
 *	backtrace.hh
 *	============
 */

#ifndef RECALL_BACKTRACE_HH
#define RECALL_BACKTRACE_HH

// Recall
#include "recall/stack_crawl.hh"


namespace plus
{
	
	class var_string;
	
}

namespace recall
{
	
	void make_report_from_stack_crawl( plus::var_string&  result,
	                                   const frame_data*  begin,
	                                   const frame_data*  end );
	
	class debugging_context
	{
		private:
			static const unsigned frame_capacity = 64;
			
			frame_data  its_stack_crawl[ frame_capacity ];
			unsigned    its_n_frames;
		
		public:
			debugging_context();
			
			const frame_data* get_stack_crawl_data() const
			{
				return its_stack_crawl;
			}
			
			unsigned get_stack_crawl_size() const
			{
				return its_n_frames;
			}
	};
	
}

#endif

