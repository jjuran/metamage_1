/*
	cond.cc
	-------
*/

#include "poseven/types/cond.hh"

// must
#include "must/pthread.h"

// poseven
#include "poseven/types/errno_t.hh"
#include "poseven/types/mutex.hh"


namespace poseven
{
	
	cond::cond()
	{
		int err = pthread_cond_init( &its_cond, NULL );
		
		throw_errno( err );
	}
	
	cond::~cond()
	{
		must_pthread_cond_destroy( &its_cond );
	}
	
	void cond::wait( const lock& k )
	{
		must_pthread_cond_wait( &its_cond, &k.get_mutex() );
	}
	
	void cond::signal()
	{
		must_pthread_cond_signal( &its_cond );
	}
	
	void cond::broadcast()
	{
		must_pthread_cond_broadcast( &its_cond );
	}
	
}
