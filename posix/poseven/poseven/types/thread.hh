/*
	thread.hh
	---------
*/

#ifndef POSEVEN_TYPES_THREAD_HH
#define POSEVEN_TYPES_THREAD_HH

// POSIX
#include <pthread.h>

// poseven
#include "poseven/types/mutex.hh"


namespace poseven
{
	
	enum thread_status
	{
		Thread_none,
		Thread_new,
		Thread_running,
		Thread_cancelling,
		Thread_cancelled,
		Thread_failed,
		Thread_ended,
		Thread_joined,
	};
	
	typedef void* (*thread_entry_proc)( void* param );
	
	class thread
	{
		public:
			typedef void (*callback_proc)( thread& t );
			
			struct callback_set
			{
				callback_proc enter;
				callback_proc leave;
			};
		
		private:
			mutable mutex  its_mutex;
			
			thread_status  its_status;
			pthread_t      its_pthread;
			
			thread_entry_proc  its_entry;
			void*              its_param;
			
			const callback_set*  its_scope_callbacks;
			
			bool it_is_joinable;
			bool it_should_cancel;
			
			// non-copyable
			thread           ( const thread &);
			thread& operator=( const thread &);
			
			static void* start( void* param );
			
			void cancel_out_of_scope( pthread_cond_t* cond );
			
			void self_testcancel();
		
		public:
			struct never_created   {};
			struct already_created {};
			struct already_joined  {};
			
			static void set_interrupt_signal( int signum );
			
			static void set_scope_callbacks( const callback_set* callbacks );
			
			static void testcancel();
			
			thread();
			~thread();
			
			bool exists() const;
			bool is_the_current_thread() const;
			
			void create( thread_entry_proc entry, void* param );
			
			void cancel( pthread_cond_t* cond = NULL );
			void join();
	};
	
	inline
	bool thread::exists() const
	{
		return its_status != Thread_none  &&  its_status != Thread_joined;
	}
	
	inline
	bool thread::is_the_current_thread() const
	{
		lock k( its_mutex );
		
		return exists()  &&  pthread_equal( pthread_self(), its_pthread );
	}
	
}

#endif
