/*
	signal.hh
	---------
*/

#ifndef RELIX_SIGNAL_SIGNAL_HH
#define RELIX_SIGNAL_SIGNAL_HH

// POSIX
#ifdef __RELIX__
#include <sys/signal.h>
#else
#include <signal.h>
#endif


namespace relix
{
	
	struct signal
	{
		int               number;
		struct sigaction  action;
	};
	
	extern signal the_signal;
	
	inline void prevent_syscall_restart()
	{
		the_signal.action.sa_flags &= ~SA_RESTART;
	}
	
}

#endif
