/*
	current_stack_base.cc
	---------------------
*/

#include "relix/api/current_stack_base.hh"

// Relix
#include "relix/api/current_thread.hh"
#include "relix/task/syscall_stack_base.hh"
#include "relix/task/thread.hh"


void* current_stack_base()
{
	void* const memory = relix::current_thread().get_syscall_stack_memory();
	
	if ( memory == NULL )
	{
		return NULL;
	}
	
	return relix::base_address_of_syscall_stack( memory );
}
