/*
	current_stack_base.cc
	---------------------
*/

#include "relix/api/current_stack_base.hh"

// Relix
#include "relix/api/current_thread.hh"
#include "relix/task/thread_syscall_stack_base.hh"


void* current_stack_base()
{
	return relix::base_address_of_syscall_stack( relix::current_thread() );
}
