/*
	caught_signal.hh
	----------------
*/

#ifndef RELIX_SIGNAL_CAUGHTSIGNAL_HH
#define RELIX_SIGNAL_CAUGHTSIGNAL_HH

// POSIX
#include <sys/signal.h>


namespace relix
{
	
	struct caught_signal
	{
		int               signo;
		struct sigaction  action;
	};
	
	extern caught_signal the_caught_signal;
	
	inline void prevent_syscall_restart()
	{
		the_caught_signal.action.sa_flags &= ~SA_RESTART;
	}
	
}

#endif

