/*
	signal.hh
	---------
*/

#ifndef RELIX_SIGNAL_SIGNAL_HH
#define RELIX_SIGNAL_SIGNAL_HH

// POSIX
#include <sys/signal.h>


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
