/*
	syscall_stack.hh
	----------------
*/

#ifndef RELIX_TASK_SYSCALLSTACK_HH
#define RELIX_TASK_SYSCALLSTACK_HH

// iota
#include "iota/class.hh"


namespace relix
{
	
	class syscall_stack
	{
		NON_COPYABLE( syscall_stack )
		NO_NEW_DELETE
		
		public:
			void* memory;
		
		public:
			void allocate();
			
			syscall_stack() : memory()
			{
			}
			
			~syscall_stack();
	};
	
}

#endif
