/*
	thread_syscall_stack_base.hh
	----------------------------
*/

#ifndef RELIX_TASK_THREADSYSCALLSTACKBASE_HH
#define RELIX_TASK_THREADSYSCALLSTACKBASE_HH

// Relix
#include "relix/task/syscall_stack_base.hh"
#include "relix/task/thread.hh"


namespace relix
{
	
	inline
	void* base_address_of_syscall_stack( const thread& thr )
	{
		void* const memory = thr.get_syscall_stack_memory();
		
		return memory ? base_address_of_syscall_stack( memory ) : 0;
	}
	
}

#endif
