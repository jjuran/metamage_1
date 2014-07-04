/*
	mmap.hh
	-------
*/

#ifndef RELIX_SYSCALL_MMAP_HH
#define RELIX_SYSCALL_MMAP_HH


namespace relix
{
	
	long _relix_mmap( void *addr, unsigned long len, int prot, int flags, int fd, long off );
	
}

#endif
