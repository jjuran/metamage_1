/*
	thread.cc
	----------
*/

#include "relix/task/thread.hh"


namespace relix
{
	
	thread::thread( int id )
	:
		its_id( id ),
		its_pending_signals(),
		its_blocked_signals()
	{
	}
	
	thread::thread( int id, const thread& caller )
	:
		its_id( id ),
		its_pending_signals(),  // reset pending signals on new threads
		its_blocked_signals( caller.its_blocked_signals )
	{
	}
	
	thread::~thread()
	{
	}
	
}

