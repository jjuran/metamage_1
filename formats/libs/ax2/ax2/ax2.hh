/*
	ax2.hh
	------
*/

#ifndef AX2_AX2_HH
#define AX2_AX2_HH

// POSIX
#include <sys/types.h>


namespace ax2
{
	
	ssize_t get_FInfo( int fd, void*       buffer, size_t length );
	ssize_t set_FInfo( int fd, void const* buffer, size_t length );
	
	off_t geteof_rsrc( int fd );
	
	int seteof_rsrc( int fd, off_t eof );
	
	ssize_t slurp_rsrc( int fd, void*       p, size_t n );
	ssize_t splat_rsrc( int fd, void const* p, size_t n );
	
	ssize_t pread_rsrc ( int fd, void*       p, size_t n, off_t offset );
	ssize_t pwrite_rsrc( int fd, void const* p, size_t n, off_t offset );
	
}

#endif
