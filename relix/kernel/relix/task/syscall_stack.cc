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
	
	syscall_stack::syscall_stack( bool allocating )
	:
		memory( allocating ? ::operator new( syscall_stack_size ) : NULL )
	{
		if ( allocating )
		{
			memset( memory, '\0', syscall_stack_size );
		}
	}
	
	syscall_stack::~syscall_stack()
	{
		::operator delete( memory );
	}
	
}

