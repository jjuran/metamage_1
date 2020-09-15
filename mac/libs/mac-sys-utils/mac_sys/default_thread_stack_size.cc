/*
	default_thread_stack_size.hh
	----------------------------
*/

#include "mac_sys/default_thread_stack_size.hh"

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// Mac OS
#ifndef __THREADS__
#include <Threads.h>
#endif


namespace mac {
namespace sys {
	
	long default_thread_stack_size()
	{
		::Size size = 0;
		
		// Jaguar returns paramErr
		OSStatus err = ::GetDefaultThreadStackSize( kCooperativeThread, &size );
		
		return size;
	}
	
}
}
