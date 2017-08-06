/*
	os_thread_api.cc
	----------------
*/

#include "relix/api/os_thread_api.hh"

// cthread-system
#include "cthread-system.hh"


namespace relix
{
	
	using namespace cthread::system;
	
	
	bool is_os_thread_stopped( os_thread_id id )
	{
		return is_thread_stopped( id );
	}
	
	void stop_os_thread( os_thread_id id )
	{
		stop_thread( id );
	}
	
	void wake_os_thread( os_thread_id id )
	{
		wake_thread( id );
	}
	
	void os_thread_yield()
	{
		thread_yield();
	}
	
}
