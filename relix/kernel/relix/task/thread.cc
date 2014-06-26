/*
	thread.cc
	----------
*/

#include "relix/task/thread.hh"

// relix
#include "relix/api/get_thread.hh"
#include "relix/task/process.hh"


namespace relix
{
	
	thread::thread( int id, sigset_t blocked, process& p, bool use_syscall_stack )
	:
		its_id( id ),
		its_pending_signals(),  // reset pending signals on new threads
		its_blocked_signals( blocked ),
		its_process( &p ),
	#if CONFIG_SYSCALL_STACKS
		its_syscall_stack( use_syscall_stack ),
	#endif
		its_stack_frame_ptr()
	{
	}
	
	thread::~thread()
	{
	}
	
	process& thread::get_process() const
	{
		return *its_process;
	}
	
	void thread::mark_current_stack_frame()
	{
		const int levels_to_skip = 2;
		
		/*
			skip recall::get_stack_frame_pointer()
			skip mark_current_stack_frame()
		*/
		
		its_stack_frame_ptr = recall::get_stack_frame_pointer( levels_to_skip );
	}
	
	os_thread_id thread::get_os_thread() const
	{
		const thread* next_thread = this;
		
		while ( next_thread->its_os_thread.get() == 0 )
		{
			pid_t ppid = next_thread->get_process().getppid();
			
			if ( ppid == 1 )
			{
				return 0;
			}
			
			next_thread = &get_thread( ppid );
		}
		
		return get_os_thread_id( *next_thread->its_os_thread.get() );
	}
	
}

