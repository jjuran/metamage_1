/*
	os_thread_api.cc
	----------------
*/

#include "relix/api/os_thread_api.hh"

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// Mac OS
#ifndef __THREADS__
#include <Threads.h>
#endif

// Nitrogen
#include "Mac/Toolbox/Utilities/ThrowOSStatus.hh"


namespace relix
{
	
	bool is_os_thread_stopped( os_thread_id id )
	{
		::ThreadState state;
		
		OSErr err = ::GetThreadState( id, &state );
		
		Mac::ThrowOSStatus( err );
		
		return state == kStoppedThreadState;
	}
	
	void stop_os_thread( os_thread_id id )
	{
		OSErr err = ::SetThreadState( id, kStoppedThreadState, kNoThreadID );
		
		Mac::ThrowOSStatus( err );
	}
	
	void wake_os_thread( os_thread_id id )
	{
		OSErr err = ::SetThreadState( id, kReadyThreadState, kNoThreadID );
		
		Mac::ThrowOSStatus( err );
	}
	
	void os_thread_yield()
	{
		// Ignore errors so we don't throw in critical sections
		(void) ::YieldToAnyThread();
	}
	
}

