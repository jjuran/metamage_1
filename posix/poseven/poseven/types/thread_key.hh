/*
	thread_key.hh
	-------------
*/

#ifndef POSEVEN_TYPES_THREADKEY_HH
#define POSEVEN_TYPES_THREADKEY_HH

// POSIX
#include <pthread.h>

// must
#include "must/pthread.h"

// poseven
#include "poseven/types/errno_t.hh"


namespace poseven
{
	
	template < class T >
	class thread_key
	{
		private:
			pthread_key_t its_key;
			
			// non-copyable
			thread_key           ( const thread_key& );
			thread_key& operator=( const thread_key& );
		
		public:
			typedef T value_type;
			
			thread_key()
			{
				int err = pthread_key_create( &its_key, NULL );
				
				throw_errno( err );
			}
			
			~thread_key()
			{
				must_pthread_key_delete( its_key );
			}
			
			value_type getspecific() const
			{
				return (value_type) pthread_getspecific( its_key );
			}
			
			void setspecific( value_type value ) const
			{
				pthread_setspecific( its_key, (void*) value );
			}
	};
	
}

#endif
