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
		Thread_failed,
		Thread_ended,
		Thread_joined,
	};
	
	typedef void* (*thread_entry_proc)( void* param );
	
	class thread
	{
		private:
			mutable mutex  its_mutex;
			
			thread_status  its_status;
			pthread_t      its_pthread;
			
			thread_entry_proc  its_entry;
			void*              its_param;
			
			// non-copyable
			thread           ( const thread &);
			thread& operator=( const thread &);
			
			static void* start( void* param );
		
		public:
			struct never_created   {};
			struct already_created {};
			struct already_joined  {};
			
			thread();
			~thread();
			
			bool exists() const;
			
			void create( thread_entry_proc entry, void* param );
			
			void join();
	};
	
	inline
	bool thread::exists() const
	{
		return its_status != Thread_none  &&  its_status != Thread_joined;
	}
	
}

#endif
