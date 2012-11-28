/*
	stack.cc
	--------
*/

#include "Genie/api/stack.hh"

// Relix
#include "relix/task/syscall_stack_base.hh"

// Debug
#include "debug/assert.hh"

// Genie
#include "Genie/current_process.hh"
#include "Genie/Process.hh"


void* current_stack_base()
{
	void* const memory = Genie::current_process().get_syscall_stack_memory();
	
	ASSERT( memory != NULL );
	
	return relix::base_address_of_syscall_stack( memory );
}

