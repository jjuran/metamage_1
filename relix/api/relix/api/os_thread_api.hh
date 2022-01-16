/*
	os_thread_api.hh
	----------------
*/

#ifndef RELIX_OSTHREADAPI_HH
#define RELIX_OSTHREADAPI_HH


namespace relix
{
	
	typedef unsigned long os_thread_id;
	
	
	bool is_os_thread_stopped( os_thread_id id );
	
	short stop_os_thread_nothrow( os_thread_id id );
	
	bool woken_os_thread( os_thread_id id );
	
	void os_thread_yield();
	
}

#endif
