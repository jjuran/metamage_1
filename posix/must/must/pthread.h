/*
	must/pthread.h
	--------------
*/

#ifndef MUST_PTHREAD_H
#define MUST_PTHREAD_H

// POSIX
#include <pthread.h>


#ifdef __cplusplus
extern "C" {
#endif


void must_pthread_mutex_lock  ( pthread_mutex_t* mutex );
void must_pthread_mutex_unlock( pthread_mutex_t* mutex );


#ifdef __cplusplus
}
#endif


#endif
