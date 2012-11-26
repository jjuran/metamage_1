/*
	syscall_stack_base.hh
	---------------------
*/

#ifndef RELIX_TASK_SYSCALLSTACKBASE_HH
#define RELIX_TASK_SYSCALLSTACKBASE_HH

// Relix
#include "relix/task/syscall_stack_size.hh"


namespace relix
{
	
	inline void* base_address_of_syscall_stack( void* memory )
	{
		return (char*) memory + syscall_stack_size;
	}
	
}

#endif

