/*
	msync.hh
	--------
*/

#ifndef RELIX_SYSCALL_MSYNC_HH
#define RELIX_SYSCALL_MSYNC_HH


namespace relix
{
	
	int msync( void* addr, unsigned long len, int flags );
	
}

#endif
