/*
	scheduler.cc
	------------
*/

#include "Genie/scheduler.hh"

// Standard C++
#include <set>


namespace Genie
{
	
	static std::set< pid_t > active_processes;
	
	bool is_active()
	{
		return !active_processes.empty();
	}
	
	void mark_process_active( pid_t pid )
	{
		active_processes.insert( pid );
	}
	
	void mark_process_inactive( pid_t pid )
	{
		active_processes.erase( pid );
	}
	
}

