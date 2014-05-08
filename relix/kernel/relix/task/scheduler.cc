/*
	scheduler.cc
	------------
*/

#include "relix/task/scheduler.hh"

// Standard C++
#include <set>


namespace relix
{
	
	static std::set< pid_t > active_threads;
	
	bool is_active()
	{
		return !active_threads.empty();
	}
	
	void mark_thread_active( pid_t tid )
	{
		active_threads.insert( tid );
	}
	
	void mark_thread_inactive( pid_t tid )
	{
		active_threads.erase( tid );
	}
	
}
