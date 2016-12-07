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
	
	struct thread_cancelled {};
	
	static thread_key< thread* > thread_object_key;
	
	static thread_key< const thread::callback_set* > scope_callbacks_key;
	
	static int interrupt_signal = 0;
	
	
	void thread::set_interrupt_signal( int signum )
	{
		interrupt_signal = signum;
	}
	
	void thread::set_scope_callbacks( const thread::callback_set* callbacks )
	{
		scope_callbacks_key.setspecific( callbacks );
	}
	
	void* thread::start( void* param )
	{
		thread& that = *(thread*) param;
		
		{
			lock k( that.its_mutex );
			
			that.its_status = Thread_running;
		}
		
		thread_object_key.setspecific( &that );
		
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
	
	thread::thread() : its_status(), it_should_cancel()
	{
	}
	
	thread::~thread()
	{
		ASSERT( ! exists() );
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
	}
	
	void thread::cancel()
	{
		const callback_set* scope = scope_callbacks_key.getspecific();
		
		if ( scope )
		{
			scope->leave( *this );
		}
		
		cancel_out_of_scope();
		
		if ( scope )
		{
			scope->enter( *this );
		}
	}
	
	void thread::cancel_out_of_scope()
	{
		lock k( its_mutex );
		
		it_should_cancel = true;
		
		while ( its_status == Thread_running )
		{
			unlock uk( its_mutex );
			
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
		if ( thread* that = thread_object_key.getspecific() )
		{
			that->self_testcancel();
		}
	}
	
	void thread::join()
	{
		lock k( its_mutex );
		
		if ( its_status == Thread_none )
		{
			throw never_created();
		}
		
		if ( its_status == Thread_joined )
		{
			throw already_joined();
		}
		
		{
			unlock uk( its_mutex );
			
			int err = pthread_join( its_pthread, NULL );
			
			throw_errno( err );
		}
		
		its_status = Thread_joined;
	}
	
}
