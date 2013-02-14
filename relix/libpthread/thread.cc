/*
	thread.cc
	---------
*/

// POSIX
#include <errno.h>
#include <pthread.h>
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
	
	void run()
	{
		result = start( arg );
	}
};

static pthread pthreads[ PTHREAD_THREADS_MAX ];

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
	
	that->run();
	
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

