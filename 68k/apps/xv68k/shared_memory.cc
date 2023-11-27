/*
	shared_memory.cc
	----------------
*/

#include "shared_memory.hh"

// POSIX
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>


void* open_shared_memory( const char* path )
{
	int fd = open( path, O_RDWR );
	
	if ( fd < 0 )
	{
		return NULL;
	}
	
	void* addr = NULL;
	
	struct stat st;
	
	int nok = fstat( fd, &st );
	
	if ( nok )
	{
		goto end;
	}
	
	size_t size;
	
	size = st.st_size;
	
	addr = mmap( NULL,
				 size,
				 PROT_READ | PROT_WRITE,
				 MAP_SHARED,
				 fd,
				 0 );
	
end:
	
	int saved_errno = errno;
	
	close( fd );
	
	errno = saved_errno;
	
	return addr;
}
