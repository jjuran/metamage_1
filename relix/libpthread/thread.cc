/*
	thread.cc
	---------
*/

// POSIX
#include <errno.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>

// relix
#include "relix/sched.h"


#ifndef PTHREAD_THREADS_MAX
#define PTHREAD_THREADS_MAX  64
#endif


struct pthread
{
	pid_t tid;
	void* (*start)(void*);
	void* arg;
	void* result;
};

static pthread pthreads[ PTHREAD_THREADS_MAX ];

static int n_keys = 0;

static void init_thread_leader()
{
	if ( pthreads[ 0 ].tid == 0 )
	{
		pthreads[ 0 ].tid = gettid();
	}
}

static int pthread_clone_function( void* arg )
{
	pthread* that = (pthread*) arg;
	
	arg = that->arg;
	
	that->arg = NULL;
	
	that->result = that->start( arg );
	
	return 0;
}

static int find_unused_pthread_id()
{
	init_thread_leader();
	
	for ( int i = 0;  i < PTHREAD_THREADS_MAX;  ++i )
	{
		if ( pthreads[ i ].tid == 0 )
		{
			return i;
		}
	}
	
	return -1;
}

pthread_t pthread_self()
{
	init_thread_leader();
	
	{
		int tid = gettid();
		
		for ( int i = 0;  i < PTHREAD_THREADS_MAX;  ++i )
		{
			if ( pthreads[ i ].tid == tid )
			{
				return i;
			}
		}
	}
	
	return 0;
}

int pthread_equal( pthread_t a, pthread_t b )
{
	return a == b;
}

int pthread_create( pthread_t*             thread_id,
                    const pthread_attr_t*  attr,
                    void*                  (*start_routine)(void*),
                    void*                  arg)
{
	int id = find_unused_pthread_id();
	
	if ( id < 0 )
	{
		return EAGAIN;
	}
	
	pthread* thread = &pthreads[ id ];
	
	thread->start  = start_routine;
	thread->arg    = arg;
	thread->result = NULL;
	
	const int flags = CLONE_VM
	                | CLONE_FS
	                | CLONE_FILES
	                | CLONE_SIGHAND
	                | CLONE_PARENT
	                | CLONE_THREAD;
	
	int tid = _relix_clone( pthread_clone_function, NULL, 0, flags, thread );
	
	if ( tid < 0 )
	{
		return errno;
	}
	
	thread->tid = tid;
	
	*thread_id = id;
	
	return 0;
}

int pthread_join( pthread_t id, void** result )
{
	if ( pid_t tid = pthreads[ id ].tid )
	{
		int status;
		
		int waited = waitpid( tid, &status, __WTHREAD );
		
		if ( waited >= 0 )
		{
			if ( result )
			{
				*result = pthreads[ id ].result;
			}
			
			pthreads[ id ].tid = 0;
			
			return 0;
		}
		
		return EINVAL;
	}
	
	return ESRCH;
}

int pthread_kill( pthread_t id, int sig )
{
	pid_t tid;
	
	if ( ! (tid = pthreads[ id ].tid) )
	{
		return ESRCH;
	}
	
	if ( sig < 0 )
	{
		return EINVAL;
	}
	
	int nok = kill( tid, sig | __SIGTHREAD );
	
	return nok ? errno : 0;
}

int pthread_key_create( pthread_key_t* key, void (*destructor_function)(void*) )
{
	if ( n_keys > 0 )
	{
		return EAGAIN;
	}
	
	n_keys = 1;
	
	*key = 0;
	
	return 0;
}

int pthread_key_delete( pthread_key_t )
{
	n_keys = 0;
	
	return 0;
}

int pthread_setspecific( pthread_key_t key, const void* value )
{
	const int i = pthread_self();
	
	pthreads[ i ].arg = (void*) value;
	
	return 0;
}

void* pthread_getspecific( pthread_key_t key )
{
	const int i = pthread_self();
	
	return pthreads[ i ].arg;
}
