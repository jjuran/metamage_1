/*
	thread.cc
	----------
*/

#include "relix/task/thread.hh"

// relix
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
		const int levels_to_skip = 1;
		
		its_stack_frame_ptr = recall::get_stack_frame_pointer( levels_to_skip );
	}
	
}

