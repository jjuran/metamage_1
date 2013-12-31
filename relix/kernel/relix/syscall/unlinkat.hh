/*
	unlinkat.hh
	-----------
*/

#ifndef RELIX_SYSCALL_UNLINKAT_HH
#define RELIX_SYSCALL_UNLINKAT_HH


namespace relix
{
	
	int unlinkat( int dirfd, const char* path, int flags );
	
}

#endif

