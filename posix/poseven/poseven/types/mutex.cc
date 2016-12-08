/*
	mutex.cc
	--------
*/

#include "poseven/types/mutex.hh"

// must
#include "must/pthread.h"

// poseven
#include "poseven/types/errno_t.hh"


namespace poseven
{
	
	mutex::mutex()
	{
		int err = pthread_mutex_init( &its_mutex, NULL );
		
		throw_errno( err );
	}
	
	mutex::~mutex()
	{
		must_pthread_mutex_destroy( &its_mutex );
	}
	
	lock::lock( mutex& m ) : its_mutex( m )
	{
		must_pthread_mutex_lock( &its_mutex.get() );
	}
	
	lock::~lock()
	{
		must_pthread_mutex_unlock( &its_mutex.get() );
	}
	
	unlock::unlock( mutex& m ) : its_mutex( m )
	{
		must_pthread_mutex_unlock( &its_mutex.get() );
	}
	
	unlock::~unlock()
	{
		must_pthread_mutex_lock( &its_mutex.get() );
	}
	
}
