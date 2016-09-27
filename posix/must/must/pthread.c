/*
	must/pthread.c
	--------------
*/

#include "must/pthread.h"

// Standard C
#include <stdlib.h>


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
