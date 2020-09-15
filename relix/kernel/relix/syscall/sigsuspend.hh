/*
	sigsuspend.hh
	-------------
*/

#ifndef RELIX_SYSCALL_SIGSUSPEND_HH
#define RELIX_SYSCALL_SIGSUSPEND_HH

// relix
#include "relix/signal/sigset_t.hh"


namespace relix
{
	
	int sigsuspend( const sigset_t* sigmask );
	
}

#endif
