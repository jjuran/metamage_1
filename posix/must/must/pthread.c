/*
	must/pthread.c
	--------------
*/

#include "must/pthread.h"

// POSIX
#include <unistd.h>


#define STR_LEN( s )  "" s, (sizeof s - 1)

#define FAILED( name )  "*** critical " #name "() failed ***\n"

#define MUST( name, args )  \
	if ( name args )  \
	{                 \
		write( STDERR_FILENO, STR_LEN( FAILED( name ) ) );  \
		_exit( 13 );                                        \
	}                 \
	else


void must_pthread_mutexattr_init( pthread_mutexattr_t* attr )
{
	MUST( pthread_mutexattr_init, ( attr ) );
}

void must_pthread_condattr_init( pthread_condattr_t* attr )
{
	MUST( pthread_condattr_init, ( attr ) );
}

void must_pthread_mutexattr_destroy( pthread_mutexattr_t* attr )
{
	MUST( pthread_mutexattr_destroy, ( attr ) );
}

void must_pthread_condattr_destroy( pthread_condattr_t* attr )
{
	MUST( pthread_condattr_destroy, ( attr ) );
}

void must_pthread_mutex_init( pthread_mutex_t* obj, pthread_mutexattr_t* attr )
{
	MUST( pthread_mutex_init, ( obj, attr ) );
}

void must_pthread_mutex_destroy( pthread_mutex_t* mutex )
{
	MUST( pthread_mutex_destroy, ( mutex ) );
}

void must_pthread_mutex_lock( pthread_mutex_t* mutex )
{
	MUST( pthread_mutex_lock, ( mutex ) );
}

void must_pthread_mutex_unlock( pthread_mutex_t* mutex )
{
	MUST( pthread_mutex_unlock, ( mutex ) );
}

void must_pthread_cond_init( pthread_cond_t* obj, pthread_condattr_t* attr )
{
	MUST( pthread_cond_init, ( obj, attr ) );
}

void must_pthread_cond_destroy( pthread_cond_t* cond )
{
	MUST( pthread_cond_destroy, ( cond ) );
}

void must_pthread_cond_wait( pthread_cond_t* cond, pthread_mutex_t* mutex )
{
	MUST( pthread_cond_wait, ( cond, mutex ) );
}

void must_pthread_cond_broadcast( pthread_cond_t* cond )
{
	MUST( pthread_cond_broadcast, ( cond ) );
}

void must_pthread_cond_signal( pthread_cond_t* cond )
{
	MUST( pthread_cond_signal, ( cond ) );
}

void must_pthread_key_delete( pthread_key_t key )
{
	MUST( pthread_key_delete, ( key ) );
}

void must_pthread_create( pthread_t*             thread,
                          pthread_attr_t const*  attrs,
                          void*                (*start)( void* ),
                          void*                  param )
{
	MUST( pthread_create, ( thread, attrs, start, param ) );
}
