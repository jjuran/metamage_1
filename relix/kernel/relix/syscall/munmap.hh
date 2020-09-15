/*
	munmap.hh
	---------
*/

#ifndef RELIX_SYSCALL_MUNMAP_HH
#define RELIX_SYSCALL_MUNMAP_HH


namespace relix
{
	
	int munmap( void *addr, unsigned long len );
	
}

#endif
