/*
	thread.cc
	---------
*/

#include "poseven/types/thread.hh"

// Standard C
#include <signal.h>

// debug
#include "debug/assert.hh"

// poseven
#include "poseven/types/errno_t.hh"
#include "poseven/types/thread_key.hh"


namespace poseven
{
	
#ifndef __RELIX__
	
	static mutex key_mutex;
	
#endif
	
	static thread_key< thread* >* any_thread_object_key;  // i.e. "if any"
	
	static
	thread_key< thread* >& the_thread_object_key()
	{
	#ifndef __RELIX__
		
		lock k( key_mutex );
		
	#endif
		
		static thread_key< thread* > key;
		
		any_thread_object_key = &key;
		
		return *any_thread_object_key;
	}
	
#ifdef __RELIX__
	
	template < class T >
	struct fake_thread_key
	{
		static T getspecific()  { return NULL; }
	};
	
	static fake_thread_key< const thread::callback_set* > scope_callbacks_key;
	
#else
	
	static thread_key< const thread::callback_set* > scope_callbacks_key;
	
#endif
	
	static int interrupt_signal = 0;
	
	
	void thread::set_interrupt_signal( int signum )
	{
		interrupt_signal = signum;
	}
	
#ifndef __RELIX__
	
	void thread::set_scope_callbacks( const thread::callback_set* callbacks )
	{
		scope_callbacks_key.setspecific( callbacks );
	}
	
#endif
	
	void* thread::start( void* param )
	{
		thread& that = *(thread*) param;
		
		{
			lock k( that.its_mutex );
			
			that.its_status = Thread_running;
		}
		
		the_thread_object_key().setspecific( &that );
		
		if ( that.its_scope_callbacks )
		{
			that.its_scope_callbacks->enter( that );
		}
		
		void*         result = NULL;
		thread_status status = Thread_ended;
		
		try
		{
			result = that.its_entry( that.its_param );
		}
		catch ( const thread_cancelled& )
		{
			status = Thread_cancelled;
		}
		catch ( ... )
		{
			status = Thread_failed;
		}
		
		if ( that.its_scope_callbacks )
		{
			that.its_scope_callbacks->leave( that );
		}
		
		{
			lock k( that.its_mutex );
			
			that.its_status = status;
		}
		
		return result;
	}
	
	thread::thread() : its_status(), it_is_joinable(), it_should_cancel()
	{
	}
	
	thread::~thread()
	{
		ASSERT( ! it_is_joinable );
	}
	
	void thread::create( thread_entry_proc entry, void* param )
	{
		lock k( its_mutex );
		
		if ( its_status != Thread_none )
		{
			throw already_created();
		}
		
		its_entry = entry;
		its_param = param;
		
		its_scope_callbacks = scope_callbacks_key.getspecific();
		
		int err = pthread_create( &its_pthread, NULL, &start, this );
		
		throw_errno( err );
		
		its_status = Thread_new;
		
		it_is_joinable = true;
	}
	
	void thread::cancel( pthread_cond_t* cond )
	{
		const callback_set* scope = scope_callbacks_key.getspecific();
		
		if ( scope )
		{
			scope->leave( *this );
		}
		
		cancel_out_of_scope( cond );
		
		if ( scope )
		{
			scope->enter( *this );
		}
	}
	
	void thread::cancel_out_of_scope( pthread_cond_t* cond )
	{
		lock k( its_mutex );
		
		it_should_cancel = true;
		
		while ( its_status == Thread_running )
		{
			unlock uk( its_mutex );
			
			if ( cond )
			{
				int err = pthread_cond_broadcast( cond );
				
				throw_errno( err );
			}
			
			if ( interrupt_signal )
			{
				int err = pthread_kill( its_pthread, interrupt_signal );
			}
			
			sched_yield();
		}
	}
	
	void thread::self_testcancel()
	{
		lock k( its_mutex );
		
		if ( it_should_cancel )
		{
			its_status = Thread_cancelling;
			
			throw thread_cancelled();
		}
	}
	
	void thread::testcancel()
	{
		if ( any_thread_object_key )
		{
			if ( thread* that = any_thread_object_key->getspecific() )
			{
				that->self_testcancel();
			}
		}
	}
	
	void thread::join()
	{
		lock k( its_mutex );
		
		if ( it_is_joinable )
		{
			// continue below
		}
		else if ( its_status == Thread_none )
		{
			throw never_created();
		}
		else
		{
			throw already_joined();
		}
		
		join_internals( k );
	}
	
	void thread::join_internals_already_locked()
	{
		it_is_joinable = false;
		
		{
			unlock uk( its_mutex );
			
			int err = pthread_join( its_pthread, NULL );
			
			throw_errno( err );
		}
		
		// The join is complete.  Broadcast the news to any waiters.
		
		its_cond.broadcast();
	}
	
	void thread::wait()
	{
		lock k( its_mutex );
		
		if ( its_status == Thread_none )
		{
			throw never_created();
		}
		
		// Thread must have been created, and therefore is (or was) joinable.
		
		if ( it_is_joinable )
		{
			// We're the first to call join() or wait().
			
			join_internals( k );
		}
		else
		{
			// Another thread joined (or waited) first.  Wait on the condvar.
			
			while ( its_status <= Thread_cancelling )  // running or cancelling
			{
				its_cond.wait( k );
			}
		}
	}
	
}
