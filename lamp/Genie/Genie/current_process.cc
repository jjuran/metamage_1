/*
	current_process.cc
	------------------
*/

#include "Genie/current_process.hh"

// relix-kernel
#include "relix/api/current_thread.hh"

// Genie
#include "Genie/Process.hh"


namespace relix
{
	
	thread& current_thread()
	{
		return Genie::current_process();
	}
	
}

