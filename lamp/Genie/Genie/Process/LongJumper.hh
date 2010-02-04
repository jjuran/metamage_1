/*	=============
 *	LongJumper.hh
 *	=============
 */

#ifndef GENIE_PROCESS_LONGJUMPER_HH
#define GENIE_PROCESS_LONGJUMPER_HH

// Lamp
#include "lamp/vfork.h"


namespace Genie
{
	
	class LongJumper
	{
		private:
			_resume_handler_t  its_resume_handler;
			_vfork_pad         its_pad;
		
		public:
			LongJumper() : its_resume_handler()
			{
			}
			
			void LongJump( int second_result );
			
			void SetLongJmp( _resume_handler_t handler, const _vfork_pad* pad )
			{
				its_resume_handler = handler;
				its_pad            = *pad;
			}
	};
	
}

#endif

