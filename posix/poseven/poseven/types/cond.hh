/*
	cond.hh
	-------
*/

#ifndef POSEVEN_TYPES_COND_HH
#define POSEVEN_TYPES_COND_HH

// POSIX
#include <pthread.h>


namespace poseven
{
	
	class lock;
	
	class cond
	{
		private:
			pthread_cond_t  its_cond;
			
			// non-copyable
			cond           ( const cond& );
			cond& operator=( const cond& );
		
		public:
			cond();
			~cond();
			
			void wait( const lock& k );
			
			void signal();
			void broadcast();
	};
	
}

#endif
