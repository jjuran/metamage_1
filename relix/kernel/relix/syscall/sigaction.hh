/*
	sigaction.hh
	------------
*/

#ifndef RELIX_SYSCALL_SIGACTION_HH
#define RELIX_SYSCALL_SIGACTION_HH


struct sigaction;


namespace relix
{
	
	int sigaction( int signo, const struct sigaction* action, struct sigaction* oldaction );
	
}

#endif
