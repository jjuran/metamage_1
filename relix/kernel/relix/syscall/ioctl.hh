/*
	ioctl.hh
	--------
*/

#ifndef RELIX_SYSCALL_IOCTL_HH
#define RELIX_SYSCALL_IOCTL_HH


namespace relix
{
	
	int ioctl( int fd, unsigned long request, int* argp );
	
}

#endif

