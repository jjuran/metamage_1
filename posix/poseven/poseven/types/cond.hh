/*
	cond.hh
	-------
*/

#ifndef POSEVEN_TYPES_COND_HH
#define POSEVEN_TYPES_COND_HH

// POSIX
#include <pthread.h>

// iota
#include "iota/class.hh"


namespace poseven
{
	
	class lock;
	
	class cond
	{
		NON_COPYABLE( cond )
		NO_NEW_DELETE
		
		private:
			pthread_cond_t  its_cond;
		
		public:
			cond();
			~cond();
			
			void wait( const lock& k );
			
			void signal();
			void broadcast();
	};
	
}

#endif
