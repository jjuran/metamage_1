/*
	vfork_context.hh
	----------------
	
	Joshua Juran
*/

#ifndef GENIE_PROCESS_VFORKCONTEXT_HH
#define GENIE_PROCESS_VFORKCONTEXT_HH


struct _vfork_pad;


namespace Genie
{
	
	class vfork_context
	{
		private:
			_vfork_pad* its_pad;
		
		public:
			vfork_context() : its_pad()
			{
			}
			
			~vfork_context();
			
			void resume_vfork( int second_result );
			
			void set_resume_handler( const _vfork_pad* pad );
	};
	
}

#endif

