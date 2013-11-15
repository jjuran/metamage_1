/*	=======
 *	mmap.cc
 *	=======
 */

// Standard C
#include <errno.h>

// POSIX
#include "sys/mman.h"

// vfs
#include "vfs/filehandle.hh"
#include "vfs/memory_mapping.hh"

// relix-kernel
#include "relix/api/get_fd_handle.hh"

// Genie
#include "Genie/current_process.hh"
#include "Genie/Process.hh"
#include "Genie/SystemCallRegistry.hh"
#include "Genie/FS/ResolvePathAt.hh"
#include "Genie/mmap/map_anonymous.hh"


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
			return set_errno( EINVAL );
		}
		
		const bool anonymous = flags & MAP_ANON;
		
		try
		{
			typedef vfs::memory_mapping_ptr  intrusive_ptr;
			typedef void*                                   addr_t;
			
			const intrusive_ptr memory = anonymous ? map_anonymous                 ( len, prot, flags      )
			                                       : relix::get_fd_handle( fd ).Map( len, prot, flags, off );
			
			const addr_t address = current_process().add_memory_mapping( memory.get() );
			
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
			current_process().remove_memory_mapping( addr );
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
			current_process().msync_memory_mapping( addr, len, flags );
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

