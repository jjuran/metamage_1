/*
	mutex.cc
	--------
*/

// POSIX
#include <errno.h>
#include <pthread.h>
#include <unistd.h>


int pthread_mutex_init( pthread_mutex_t* mutex, const pthread_mutexattr_t* attr )
{
	mutex->value = 0;
	
	return 0;
}

int pthread_mutex_destroy( pthread_mutex_t* mutex )
{
	return 0;
}

int pthread_mutex_trylock( pthread_mutex_t* mutex )
{
	// Cooperative threading
	
	if ( mutex->value )
	{
		return EBUSY;
	}
	
	mutex->value = 1;
	
	return 0;
}

int pthread_mutex_lock( pthread_mutex_t* mutex )
{
	// Cooperative threading
	
	while ( mutex->value )
	{
		sched_yield();
	}
	
	mutex->value = 1;
	
	return 0;
}

int pthread_mutex_unlock( pthread_mutex_t* mutex )
{
	// Cooperative threading
	
	mutex->value = 0;
	
	return 0;
}

