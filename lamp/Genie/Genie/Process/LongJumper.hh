/*	=============
 *	LongJumper.hh
 *	=============
 */

#ifndef GENIE_PROCESS_LONGJUMPER_HH
#define GENIE_PROCESS_LONGJUMPER_HH

// Standard C
#include <setjmp.h>


namespace Genie
{
	
	class LongJumper
	{
		public:
			typedef void (*long_jump_t)( jmp_buf*, int);
			
		private:
			long_jump_t   its_long_jump;
			jmp_buf      *its_buffer;
		
		public:
			LongJumper() : its_long_jump(), its_buffer()
			{
			}
			
			void LongJump( int second_result ) const;
			
			void SetLongJmp( long_jump_t f, jmp_buf* buffer )
			{
				its_long_jump = f;
				its_buffer    = buffer;
			}
	};
	
}

#endif

