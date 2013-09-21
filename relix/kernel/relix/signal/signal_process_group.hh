/*
	signal_process_group.hh
	-----------------------
*/

#ifndef RELIX_SIGNAL_SIGNALPROCESSGROUP_HH
#define RELIX_SIGNAL_SIGNALPROCESSGROUP_HH

// POSIX
#include <sys/types.h>


namespace relix
{
	
	void signal_process_group( int sig, pid_t pgid );
	
}

#endif

