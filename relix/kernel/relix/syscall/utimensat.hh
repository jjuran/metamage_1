/*
	utimensat.hh
	------------
*/

#ifndef RELIX_SYSCALL_UTIMENSAT_HH
#define RELIX_SYSCALL_UTIMENSAT_HH

// POSIX
//#include <time.h>
struct timespec;


namespace relix
{
	
	int utimensat( int fd, const char* path, const timespec* times, int flags );
	
}

#endif

