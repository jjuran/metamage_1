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
