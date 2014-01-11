/*
	signal.cc
	---------
*/

// POSIX
#include <errno.h>
#include <signal.h>


int pthread_sigmask( int how, const sigset_t* set, sigset_t* oldset )
{
	if ( sigprocmask( how, set, oldset ) < 0 )
	{
		return errno;
	}
	
	return 0;
}

