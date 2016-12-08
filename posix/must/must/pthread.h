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


void must_pthread_mutex_destroy( pthread_mutex_t* mutex );

void must_pthread_mutex_lock  ( pthread_mutex_t* mutex );
void must_pthread_mutex_unlock( pthread_mutex_t* mutex );

void must_pthread_cond_wait( pthread_cond_t* cond, pthread_mutex_t* mutex );

void must_pthread_cond_destroy( pthread_cond_t* cond );

void must_pthread_cond_broadcast( pthread_cond_t* cond );
void must_pthread_cond_signal   ( pthread_cond_t* cond );

void must_pthread_create( pthread_t*             thread,
                          pthread_attr_t const*  attrs,
                          void*                (*start)( void* ),
                          void*                  param );


#ifdef __cplusplus
}
#endif


#endif
