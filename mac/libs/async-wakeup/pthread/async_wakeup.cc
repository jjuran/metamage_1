/*
	async_wakeup.cc
	---------------
*/

#include "mac_sys/async_wakeup.hh"

// Mac OS X
#ifdef __APPLE__
#include <ApplicationServices/ApplicationServices.h>
#endif

// Mac OS
#ifndef __PROCESSES__
#include <Processes.h>
#endif

// POSIX
#include <pthread.h>
#include <sys/time.h>

// must
#include "must/pthread.h"

// mac-sys-utils
#include "mac_sys/current_process.hh"


namespace mac {
namespace sys {
	
	static volatile bool wakeup_pending = false;
	
	static pthread_mutex_t wakeup_mutex = PTHREAD_MUTEX_INITIALIZER;
	static pthread_cond_t  wakeup_cond  = PTHREAD_COND_INITIALIZER;
	
	static
	void* wakeup_thread_proc( void* )
	{
		const ProcessSerialNumber& psn = current_process();
		
		must_pthread_mutex_lock( &wakeup_mutex );
		
		for ( ;; )
		{
			if ( wakeup_pending )
			{
				struct timeval now;
				
				gettimeofday( &now, NULL );
				
				uint64_t s  = now.tv_sec;
				uint32_t us = now.tv_usec + 10 * 1000;
				
				if ( us >= 1000 * 1000 )
				{
					us -= 1000 * 1000;
					
					s += 1;
				}
				
				struct timespec soon = { s, us * 1000 };
				
				pthread_cond_timedwait( &wakeup_cond, &wakeup_mutex, &soon );
			}
			else
			{
				must_pthread_cond_wait( &wakeup_cond, &wakeup_mutex );
			}
			
			(void) ::WakeUpProcess( &psn );
		}
		
	//	must_pthread_mutex_unlock( &wakeup_mutex );  // not reached
	}
	
	void request_async_wakeup()
	{
		must_pthread_mutex_lock( &wakeup_mutex );
		
		wakeup_pending = true;
		
		must_pthread_cond_signal( &wakeup_cond );
		
		must_pthread_mutex_unlock( &wakeup_mutex );
	}
	
	void clear_async_wakeup()
	{
		wakeup_pending = false;
	}
	
	struct wakeup_thread
	{
		pthread_t thread;
		
		wakeup_thread();
	};
	
	wakeup_thread::wakeup_thread()
	{
		must_pthread_create( &thread, NULL, &wakeup_thread_proc, NULL );
	}
	
	static wakeup_thread the_wakeup_thread;
	
}
}
