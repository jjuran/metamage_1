/*
	mutex.hh
	--------
*/

#ifndef POSEVEN_TYPES_MUTEX_HH
#define POSEVEN_TYPES_MUTEX_HH

// POSIX
#include <pthread.h>


namespace poseven
{
	
	class lock;
	class unlock;
	
	class mutex
	{
		friend class lock;
		friend class unlock;
		
		private:
			pthread_mutex_t  its_mutex;
			
			// non-copyable
			mutex           ( const mutex& );
			mutex& operator=( const mutex& );
			
			pthread_mutex_t& get()  { return its_mutex; }
		
		public:
			mutex();
			~mutex();
	};
	
	class lock
	{
		private:
			mutex& its_mutex;
			
			// non-copyable
			lock           ( const lock& );
			lock& operator=( const lock& );
		
		public:
			lock( mutex& m );
			~lock();
	};
	
	class unlock
	{
		private:
			mutex& its_mutex;
			
			// non-copyable
			unlock           ( const unlock& );
			unlock& operator=( const unlock& );
		
		public:
			unlock( mutex& m );
			~unlock();
	};
	
}

#endif
