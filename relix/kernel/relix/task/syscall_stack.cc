/*
	syscall_stack.cc
	----------------
*/

#include "relix/task/syscall_stack.hh"

// Standard C
#include <string.h>

// Relix
#include "relix/task/syscall_stack_size.hh"


namespace relix
{
	
	void syscall_stack::allocate()
	{
		if ( ! memory )
		{
			memory = ::operator new( syscall_stack_size );
			
			memset( memory, '\0', syscall_stack_size );
		}
	}
	
	syscall_stack::~syscall_stack()
	{
		::operator delete( memory );
	}
	
}
