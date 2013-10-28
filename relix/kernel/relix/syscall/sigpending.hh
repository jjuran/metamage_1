/*
	sigpending.hh
	-------------
*/

#ifndef RELIX_SYSCALL_SIGPENDING_HH
#define RELIX_SYSCALL_SIGPENDING_HH


namespace relix
{
	
	typedef unsigned long sigset_t;
	
	int sigpending( sigset_t* oldset );
	
}

#endif

