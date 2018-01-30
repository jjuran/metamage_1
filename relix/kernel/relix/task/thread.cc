/*
	thread.cc
	----------
*/

#include "relix/task/thread.hh"

// relix
#include "relix/api/get_thread.hh"
#include "relix/task/process.hh"
#include "relix/task/process_image.hh"
#include "relix/task/scheduler.hh"


namespace relix
{
	
	thread::thread( int id, sigset_t blocked, process& p, bool use_syscall_stack )
	:
		its_id( id ),
		its_saved_errno(),
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
		mark_thread_inactive( its_id );
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
			
			try
			{
				next_thread = &get_thread( ppid );
			}
			catch ( ... )
			{
				return os_thread_id();
			}
		}
		
		return get_os_thread_id( *next_thread->its_os_thread.get() );
	}
	
	void thread::switch_in()
	{
		process& proc = get_process();
		
		proc.switch_in();
		
		proc.get_process_image().set_errno( its_saved_errno );
	}
	
	void thread::switch_out()
	{
		process& proc = get_process();
		
		its_saved_errno = proc.get_process_image().get_errno();
		
		proc.switch_out();
	}
	
}
