/*
	thread_key.hh
	-------------
*/

#ifndef POSEVEN_TYPES_THREADKEY_HH
#define POSEVEN_TYPES_THREADKEY_HH

// POSIX
#include <pthread.h>

// iota
#include "iota/class.hh"

// must
#include "must/pthread.h"

// poseven
#include "poseven/types/errno_t.hh"


namespace poseven
{
	
	template < class T >
	class thread_key
	{
		NON_COPYABLE( thread_key )
		NO_NEW_DELETE
		
		private:
			pthread_key_t its_key;
		
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
