/*
	sigpending.hh
	-------------
*/

#ifndef RELIX_SYSCALL_SIGPENDING_HH
#define RELIX_SYSCALL_SIGPENDING_HH

// relix
#include "relix/signal/sigset_t.hh"


namespace relix
{
	
	int sigpending( sigset_t* oldset );
	
}

#endif

