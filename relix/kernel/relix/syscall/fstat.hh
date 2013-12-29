/*
	fstat.hh
	--------
*/

#ifndef RELIX_SYSCALL_FSTAT_HH
#define RELIX_SYSCALL_FSTAT_HH

// POSIX
//#include <sys/stat.h>
struct stat;


namespace relix
{
	
	int fstat( int fd, struct stat* sb );
	
}

#endif

