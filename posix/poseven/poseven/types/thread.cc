/*
	thread.cc
	---------
*/

#include "poseven/types/thread.hh"

// debug
#include "debug/assert.hh"

// poseven
#include "poseven/types/errno_t.hh"


namespace poseven
{
	
	void* thread::start( void* param )
	{
		thread& that = *(thread*) param;
		
		{
			lock k( that.its_mutex );
			
			that.its_status = Thread_running;
		}
		
		void*         result = NULL;
		thread_status status = Thread_ended;
		
		try
		{
			result = that.its_entry( that.its_param );
		}
		catch ( ... )
		{
			status = Thread_failed;
		}
		
		{
			lock k( that.its_mutex );
			
			that.its_status = status;
		}
		
		return result;
	}
	
	thread::thread() : its_status()
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
		
		int err = pthread_create( &its_pthread, NULL, &start, this );
		
		throw_errno( err );
		
		its_status = Thread_new;
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
