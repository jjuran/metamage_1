/*
	os_thread_api.cc
	----------------
*/

#include "relix/api/os_thread_api.hh"

// cthread-either
#include "cthread-either.hh"


namespace relix
{
	
	using namespace cthread;
	using namespace cthread::either;
	
	
	bool is_os_thread_stopped( os_thread_id id )
	{
		return is_thread_stopped( id );
	}
	
	short stop_os_thread_nothrow( os_thread_id id )
	{
		return stop_thread_nothrow( id );
	}
	
	bool woken_os_thread( os_thread_id id )
	{
		return woken_thread( id );
	}
	
	void os_thread_yield()
	{
		thread_yield();
	}
	
}
