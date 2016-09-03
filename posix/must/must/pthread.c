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
