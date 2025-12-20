/*
	stack-space.cc
	--------------
*/

// Relix
#include "relix/parameter_block.h"
#include "relix/runctl.h"
#include "relix/stack.h"
#include "tool-runtime/parameter_block.h"


unsigned _relix_stack_space()
{
	return global_system_params->runctl( runctl_current_thread_stack_space );
}
