/*
	fstatat.hh
	----------
*/

#ifndef RELIX_SYSCALL_FSTATAT_HH
#define RELIX_SYSCALL_FSTATAT_HH

// POSIX
//#include <sys/stat.h>
struct stat;


namespace relix
{
	
	int fstatat( int dirfd, const char* path, struct stat* sb, int flags );
	
}

#endif

