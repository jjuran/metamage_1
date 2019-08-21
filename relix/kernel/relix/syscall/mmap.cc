/*
	mmap.cc
	-------
*/

#include "relix/syscall/mmap.hh"

// POSIX
#include <errno.h>
#ifdef __APPLE__
// This is needed to compile with Mac OS X 10.3's headers.
#include <sys/types.h>
#endif
#include <sys/mman.h>

// vfs
#include "vfs/memory_mapping.hh"
#include "vfs/filehandle/primitives/mmap.hh"
#include "vfs/mmap/functions/map_anonymous.hh"

// relix
#include "relix/api/current_process.hh"
#include "relix/api/errno.hh"
#include "relix/api/get_fd_handle.hh"
#include "relix/task/process.hh"
#include "relix/task/process_image.hh"


#ifndef MAP_ANON
#define MAP_ANON 0
#endif


namespace relix
{
	
	long _relix_mmap( void *addr, unsigned long len, int prot, int flags, int fd, long off )
	{
		if ( len == 0 )
		{
			return set_errno( EINVAL );
		}
		
		if ( flags & MAP_FIXED )
		{
			return set_errno( ENOTSUP );
		}
		
		const bool anonymous = flags & MAP_ANON;
		
		try
		{
			typedef vfs::memory_mapping_ptr  intrusive_ptr;
			typedef void*                    addr_t;
			
			const intrusive_ptr memory = anonymous ? vfs::map_anonymous        ( len, prot, flags      )
			                                       : mmap( &get_fd_handle( fd ), len, prot, flags, off );
			
			const addr_t address = current_process().get_process_image().add_memory_mapping( memory.get() );
			
			return (long) address;
		}
		catch ( ... )
		{
			set_errno_from_exception();
		}
		
		return (long) MAP_FAILED;
	}
	
}
