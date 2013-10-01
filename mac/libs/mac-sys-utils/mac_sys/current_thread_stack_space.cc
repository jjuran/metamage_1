/*
	current_thread_stack_space.hh
	-----------------------------
*/

#include "mac_sys/current_thread_stack_space.hh"

// Mac OS
#ifdef __MACOS__
#ifndef __THREADS__
#include <Threads.h>
#endif
#endif


namespace mac {
namespace sys {
	
	unsigned long current_thread_stack_space()
	{
	#ifdef __MACOS__
		
		const unsigned extra_stack = TARGET_CPU_68K * 10;
		
		::ThreadID thread;
		
		if ( ::GetCurrentThread( &thread ) != noErr )
		{
			return 0;
		}
		
		UInt32 stackSpace;
		
		if ( ::ThreadCurrentStackSpace( thread, &stackSpace ) != noErr )
		{
			return 0;
		}
		
		return stackSpace + extra_stack;
		
	#endif
		
		return 0;
	}
	
}
}
