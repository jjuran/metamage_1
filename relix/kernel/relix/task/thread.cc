/*
	thread.cc
	----------
*/

#include "relix/task/thread.hh"

// relix
#include "relix/task/process.hh"


namespace relix
{
	
	thread::thread( int id, sigset_t blocked, process& p )
	:
		its_id( id ),
		its_pending_signals(),  // reset pending signals on new threads
		its_blocked_signals( blocked ),
		its_process( &p )
	{
	}
	
	thread::~thread()
	{
	}
	
	process& thread::get_process() const
	{
		return *its_process;
	}
	
}

