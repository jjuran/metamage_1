/*
	unimplemented.cc
	----------------
*/

#include "relix/syscall/unimplemented.hh"

// Standard C
#include <errno.h>

// relix-kernel
#include "relix/api/errno.hh"


namespace relix
{
	
	int unimplemented()
	{
		return set_errno( ENOSYS );
	}
	
}

