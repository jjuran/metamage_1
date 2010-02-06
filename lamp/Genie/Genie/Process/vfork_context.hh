/*
	vfork_context.hh
	----------------
	
	Joshua Juran
*/

#ifndef GENIE_PROCESS_VFORKCONTEXT_HH
#define GENIE_PROCESS_VFORKCONTEXT_HH

// Recall
#include "recall/stack_crawl.hh"


struct _vfork_pad;


namespace Genie
{
	
	class vfork_context
	{
		private:
			_vfork_pad* its_pad;
		
		private:
			// Non-copyable
			vfork_context           ( const vfork_context& );
			vfork_context& operator=( const vfork_context& );
		
		public:
			vfork_context() : its_pad()
			{
			}
			
			~vfork_context();
			
			recall::stack_frame_pointer get_vfork_frame_pointer() const;
			
			void resume_vfork( int second_result );
			
			void set_resume_handler( const _vfork_pad* pad );
	};
	
}

#endif

