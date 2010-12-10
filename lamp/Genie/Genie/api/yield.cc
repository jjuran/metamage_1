/*
	yield.cc
	--------
*/

#include "Genie/api/yield.hh"

// Genie
#include "Genie/current_process.hh"
#include "Genie/Process.hh"


namespace Genie
{
	
	void yield()
	{
		current_process().Yield();
	}
	
}

