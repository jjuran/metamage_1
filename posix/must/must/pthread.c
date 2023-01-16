/*
	must/pthread.c
	--------------
*/

#include "must/pthread.h"

// Standard C
#include <stdlib.h>


void must_pthread_mutexattr_init( pthread_mutexattr_t* attr )
{
	int error = pthread_mutexattr_init( attr );
	
	if ( error )
	{
		abort();
	}
}

void must_pthread_condattr_init( pthread_condattr_t* attr )
{
	int error = pthread_condattr_init( attr );
	
	if ( error )
	{
		abort();
	}
}

void must_pthread_mutexattr_destroy( pthread_mutexattr_t* attr )
{
	int error = pthread_mutexattr_destroy( attr );
	
	if ( error )
	{
		abort();
	}
}

void must_pthread_condattr_destroy( pthread_condattr_t* attr )
{
	int error = pthread_condattr_destroy( attr );
	
	if ( error )
	{
		abort();
	}
}

void must_pthread_mutex_init( pthread_mutex_t* obj, pthread_mutexattr_t* attr )
{
	int error = pthread_mutex_init( obj, attr );
	
	if ( error )
	{
		abort();
	}
}

void must_pthread_mutex_destroy( pthread_mutex_t* mutex )
{
	int error = pthread_mutex_destroy( mutex );
	
	if ( error )
	{
		abort();
	}
}

void must_pthread_mutex_lock( pthread_mutex_t* mutex )
{
	int error = pthread_mutex_lock( mutex );
	
	if ( error )
	{
		abort();
	}
}

void must_pthread_mutex_unlock( pthread_mutex_t* mutex )
{
	int error = pthread_mutex_unlock( mutex );
	
	if ( error )
	{
		abort();
	}
}

void must_pthread_cond_init( pthread_cond_t* obj, pthread_condattr_t* attr )
{
	int error = pthread_cond_init( obj, attr );
	
	if ( error )
	{
		abort();
	}
}

void must_pthread_cond_destroy( pthread_cond_t* cond )
{
	int error = pthread_cond_destroy( cond );
	
	if ( error )
	{
		abort();
	}
}

void must_pthread_cond_wait( pthread_cond_t* cond, pthread_mutex_t* mutex )
{
	int error = pthread_cond_wait( cond, mutex );
	
	if ( error )
	{
		abort();
	}
}

void must_pthread_cond_broadcast( pthread_cond_t* cond )
{
	int error = pthread_cond_broadcast( cond );
	
	if ( error )
	{
		abort();
	}
}

void must_pthread_cond_signal( pthread_cond_t* cond )
{
	int error = pthread_cond_signal( cond );
	
	if ( error )
	{
		abort();
	}
}

void must_pthread_key_delete( pthread_key_t key )
{
	int error = pthread_key_delete( key );
	
	if ( error )
	{
		abort();
	}
}

void must_pthread_create( pthread_t*             thread,
                          pthread_attr_t const*  attrs,
                          void*                (*start)( void* ),
                          void*                  param )
{
	int error = pthread_create( thread, attrs, start, param );
	
	if ( error )
	{
		abort();
	}
}
