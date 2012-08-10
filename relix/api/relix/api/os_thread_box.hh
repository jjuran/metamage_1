/*
	os_thread_box.hh
	----------------
*/

#ifndef RELIX_API_OSTHREADBOX_HH
#define RELIX_API_OSTHREADBOX_HH

// relix-kernel
#include "relix/api/os_thread_api.hh"


namespace relix
{
	
	typedef void* (*os_thread_start_type)( void*, const void*, const void* );
	
	typedef void (*os_thread_switch_type)( void* );
	
	class os_thread;
	
	class os_thread_box
	{
		private:
			os_thread* its_thread;
		
		public:
			os_thread_box() : its_thread()
			{
			}
			
			os_thread_box( os_thread& thread );
			
			os_thread_box( const os_thread_box& that );
			
			os_thread_box& operator=( const os_thread_box& that );
			
			~os_thread_box();
			
			os_thread* get() const  { return its_thread; }
			
			void swap( os_thread_box& that );
			
			void reset()
			{
				os_thread_box().swap( *this );
			}
	};
	
	os_thread_box new_os_thread( os_thread_start_type   start,
	                             void*                  param,
	                             int                    stack_size,
	                             os_thread_switch_type  switch_in  = 0,
	                             os_thread_switch_type  switch_out = 0 );
	
	os_thread_id get_os_thread_id( const os_thread& thread );
	
}

#endif

