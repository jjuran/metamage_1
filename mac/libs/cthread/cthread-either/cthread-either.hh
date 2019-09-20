/*
	cthread-either.hh
	-----------------
*/

#ifndef CTHREADEITHER_HH
#define CTHREADEITHER_HH

// mac-config
#include "mac_config/threads.hh"


#if ! CONFIG_THREAD_MANAGER
	#ifndef __MC68K__
		#error  Custom threads are only supported on 68K
	#endif
	#define CTHREAD_CUSTOM  1
#elif CONFIG_THREAD_MANAGER_GRANTED  ||  ! defined( __MC68K__ )
	/*
		Assume Thread Manager is available in PPC builds.
		This is true for System 7.5+, or System 7.1.2 with the extension.
		
		TODO:  Add startup check in case Thread Manager /isn't/ available.
	*/
	#define CTHREAD_SYSTEM  1
#else
	#define CTHREAD_EITHER  1
#endif

#if CTHREAD_SYSTEM
// cthread-system
#include "cthread-system.hh"
#endif

#if CTHREAD_CUSTOM
// cthread-custom
#include "cthread-custom.hh"
#endif


namespace cthread
{
	
	struct parameter_block;
	
	typedef unsigned long thread_id;
	
#if CTHREAD_SYSTEM
	
	namespace either = system;
	
#endif
	
#if CTHREAD_CUSTOM
	
	namespace either = custom;
	
#endif
	
#if CTHREAD_EITHER

namespace either
{
	
	using cthread::parameter_block;
	
	void thread_yield();
	
	unsigned long current_thread_stack_space();
	
	thread_id current_thread();
	
	thread_id create_thread( parameter_block& pb, unsigned stack_size );
	
	void destroy_thread( thread_id id );
	
	bool is_thread_stopped( thread_id id );
	
	void stop_thread( thread_id id );
	void wake_thread( thread_id id );
	
	bool woken_thread( thread_id id );
	
	void yield_to_thread( thread_id id );
	
}

#endif  // #if CTHREAD_EITHER

}

#endif
