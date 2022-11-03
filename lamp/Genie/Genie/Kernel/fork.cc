/*
	fork.cc
	-------
*/

// POSIX
#include <unistd.h>

// Genie
#include "Genie/current_process.hh"
#include "Genie/Process.hh"


#ifndef __MACOS__
namespace {
#endif

pid_t fork()
{
	using namespace Genie;
	
	try
	{
		return current_process().fork();
	}
	catch ( ... )
	{
		return set_errno_from_exception();
	}
	
	return 0;
}


#ifndef __MACOS__
}
#endif

