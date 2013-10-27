/*
	sigprocmask.hh
	--------------
*/

#ifndef RELIX_SYSCALL_PTHREADSIGMASK_HH
#define RELIX_SYSCALL_PTHREADSIGMASK_HH


namespace relix
{
	
	typedef unsigned long sigset_t;
	
	int sigprocmask( int how, const sigset_t* set, sigset_t* oldset );
	
}

#endif

