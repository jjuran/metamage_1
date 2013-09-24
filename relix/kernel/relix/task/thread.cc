/*
	thread.cc
	----------
*/

#include "relix/task/thread.hh"

// relix
#include "relix/task/process.hh"
#include "relix/task/process_group.hh"
#include "relix/task/session.hh"


namespace relix
{
	
	static boost::intrusive_ptr< session > new_session( pid_t sid )
	{
		return new session( sid );
	}
	
	static boost::intrusive_ptr< process_group >
	//
	new_process_group( pid_t pgid, session& session )
	{
		return new process_group( pgid, session );
	}
	
	thread::thread( int id )
	:
		its_id     ( id ),
		its_pending_signals(),
		its_blocked_signals(),
		its_process( new process( id, *new_process_group( id, *new_session( id ) ) ) )
	{
	}
	
	thread::thread( int id, const thread& caller, process* p )
	:
		its_id( id ),
		its_pending_signals(),  // reset pending signals on new threads
		its_blocked_signals( caller.its_blocked_signals ),
		its_process( p ? p : new process( id, caller.get_process().get_process_group() ) )
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

