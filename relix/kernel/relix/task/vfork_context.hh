/*
	vfork_context.hh
	----------------
*/

#ifndef RELIX_TASK_VFORKCONTEXT_HH
#define RELIX_TASK_VFORKCONTEXT_HH


struct _vfork_pad;


namespace relix
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
			
			const void* get_vfork_frame_pointer() const;
			
			void resume_vfork( int second_result );
			
			void set_resume_handler( const _vfork_pad* pad );
	};
	
}

#endif

