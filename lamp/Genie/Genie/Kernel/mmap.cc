/*	=======
 *	mmap.cc
 *	=======
 */

// Standard C
#include <errno.h>

// POSIX
#include "sys/mman.h"

// vfs
#include "vfs/memory_mapping.hh"
#include "vfs/filehandle/primitives/mmap.hh"
#include "vfs/mmap/functions/map_anonymous.hh"

// relix-kernel
#include "relix/api/current_process.hh"
#include "relix/api/get_fd_handle.hh"
#include "relix/task/process.hh"
#include "relix/task/process_image.hh"

// Genie
#include "Genie/current_process.hh"
#include "Genie/SystemCallRegistry.hh"


namespace Genie
{
	
	static long _relix_mmap( void *addr, size_t len, int prot, int flags, int fd, off_t off )
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
			typedef void*                                   addr_t;
			
			const intrusive_ptr memory = anonymous ? vfs::map_anonymous            ( len, prot, flags      )
			                                       : mmap( &relix::get_fd_handle( fd ), len, prot, flags, off );
			
			const addr_t address = relix::current_process().get_process_image().add_memory_mapping( memory.get() );
			
			return (long) address;
		}
		catch ( ... )
		{
			set_errno_from_exception();
		}
		
		return (long) MAP_FAILED;
	}
	
	static int munmap( void *addr, size_t len )
	{
		if ( len == 0 )
		{
			return set_errno( EINVAL );
		}
		
		try
		{
			relix::current_process().get_process_image().remove_memory_mapping( addr );
		}
		catch ( ... )
		{
			return set_errno_from_exception();
		}
		
		return 0;
	}
	
	static int msync( void* addr, size_t len, int flags )
	{
		try
		{
			relix::current_process().get_process_image().msync_memory_mapping( addr, len, flags );
		}
		catch ( ... )
		{
			return set_errno_from_exception();
		}
		
		return 0;
	}
	
	#pragma force_active on
	
	REGISTER_SYSTEM_CALL( _relix_mmap );
	REGISTER_SYSTEM_CALL( munmap      );
	REGISTER_SYSTEM_CALL( msync       );
	
	#pragma force_active reset
	
}

