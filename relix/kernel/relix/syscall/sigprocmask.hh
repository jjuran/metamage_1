/*
	sigprocmask.hh
	--------------
*/

#ifndef RELIX_SYSCALL_SIGPROCMASK_HH
#define RELIX_SYSCALL_SIGPROCMASK_HH

// relix
#include "relix/signal/sigset_t.hh"


namespace relix
{
	
	int sigprocmask( int how, const sigset_t* set, sigset_t* oldset );
	
}

#endif

