/*
	fcntl.hh
	--------
*/

#ifndef RELIX_SYSCALL_FCNTL_HH
#define RELIX_SYSCALL_FCNTL_HH


namespace relix
{
	
	int fcntl( int fd, int cmd, int param );
	
}

#endif

