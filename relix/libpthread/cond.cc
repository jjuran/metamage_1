/*
	cond.cc
	-------
*/

// POSIX
#include <errno.h>
#include <pthread.h>

// relix
#include "deadline.hh"


int pthread_cond_init( pthread_cond_t* cond, const pthread_condattr_t* attr )
{
	cond->value = 0;
	
	return 0;
}

int pthread_cond_destroy( pthread_cond_t* cond )
{
	return 0;
}

int pthread_cond_wait( pthread_cond_t* cond, pthread_mutex_t* mutex )
{
	// Cooperative threading
	
	int err = pthread_mutex_unlock( mutex );
	
	const int t = cond->value;
	
	do
	{
		sched_yield();
	}
	while ( cond->value == t );
	
	err = pthread_mutex_lock( mutex );
	
	return 0;
}

int pthread_cond_timedwait( pthread_cond_t*         cond,
                            pthread_mutex_t*        mutex,
                            const struct timespec*  deadline )
{
	if ( (__uint32_t) deadline->tv_nsec >= 1000000000 )
	{
		return EINVAL;
	}
	
	int result = 0;
	
	// Cooperative threading
	
	int err = pthread_mutex_unlock( mutex );
	
	const int t = cond->value;
	
	do
	{
		if ( deadline_expired( *deadline ) )
		{
			result = ETIMEDOUT;
			
			break;
		}
		
		sched_yield();
	}
	while ( cond->value == t );
	
	err = pthread_mutex_lock( mutex );
	
	return result;
}

int pthread_cond_signal( pthread_cond_t* cond )
{
	// Cooperative threading
	
	++cond->value;
	
	return 0;
}

int pthread_cond_broadcast( pthread_cond_t* cond )
{
	// Cooperative threading
	
	++cond->value;
	
	return 0;
}
