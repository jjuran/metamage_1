/*
	mutex.hh
	--------
*/

#ifndef POSEVEN_TYPES_MUTEX_HH
#define POSEVEN_TYPES_MUTEX_HH

// POSIX
#include <pthread.h>

// iota
#include "iota/class.hh"


namespace poseven
{
	
	class cond;
	class lock;
	class unlock;
	
	class mutex
	{
		friend class lock;
		friend class unlock;
		
		NON_COPYABLE( mutex )
		NO_NEW_DELETE
		
		private:
			pthread_mutex_t  its_mutex;
			
			pthread_mutex_t& get()  { return its_mutex; }
		
		public:
			mutex();
			~mutex();
	};
	
	class lock
	{
		friend class cond;
		
		NON_COPYABLE( lock )
		NO_NEW_DELETE
		
		private:
			mutex& its_mutex;
			
			pthread_mutex_t& get_mutex() const  { return its_mutex.get(); }
		
		public:
			lock( mutex& m );
			~lock();
	};
	
	class unlock
	{
		NON_COPYABLE( unlock )
		NO_NEW_DELETE
		
		private:
			mutex& its_mutex;
		
		public:
			unlock( mutex& m );
			~unlock();
	};
	
}

#endif
