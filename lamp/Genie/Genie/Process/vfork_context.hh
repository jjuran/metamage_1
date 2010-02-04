/*
	vfork_context.hh
	----------------
	
	Joshua Juran
*/

#ifndef GENIE_PROCESS_VFORKCONTEXT_HH
#define GENIE_PROCESS_VFORKCONTEXT_HH

// Lamp
#include "lamp/vfork.h"


namespace Genie
{
	
	class vfork_context
	{
		private:
			_resume_handler_t  its_resume_handler;
			_vfork_pad         its_pad;
		
		public:
			vfork_context() : its_resume_handler()
			{
			}
			
			void resume_vfork( int second_result );
			
			void set_resume_handler( _resume_handler_t handler, const _vfork_pad* pad )
			{
				its_resume_handler = handler;
				its_pad            = *pad;
			}
	};
	
}

#endif

