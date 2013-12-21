/*
	socketpair.hh
	-------------
*/

#ifndef RELIX_SYSCALL_SOCKETPAIR_HH
#define RELIX_SYSCALL_SOCKETPAIR_HH


namespace relix
{
	
	int socketpair( int domain, int type, int protocol, int fds[2] );
	
}

#endif

