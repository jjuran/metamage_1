/*	=======
 *	mmap.cc
 *	=======
 */

// Standard C
#include <errno.h>

// POSIX
#include "sys/mman.h"

// Genie
#include "Genie/current_process.hh"
#include "Genie/FileDescriptors.hh"
#include "Genie/Process.hh"
#include "Genie/SystemCallRegistry.hh"
#include "Genie/SystemCalls.hh"
#include "Genie/FS/ResolvePathAt.hh"
#include "Genie/mmap/map_anonymous.hh"
#include "Genie/mmap/memory_mapping.hh"


namespace Genie
{
	
	static long _lamp_mmap( void *addr, size_t len, int prot, int flags, int fd, off_t off )
	{
		SystemCallFrame frame( "mmap" );
		
		if ( len == 0 )
		{
			frame.SetErrno( EINVAL );
		}
		
		if ( flags & MAP_FIXED )
		{
			frame.SetErrno( EINVAL );
		}
		
		const bool anonymous = flags & MAP_ANON;
		
		try
		{
			typedef boost::intrusive_ptr< memory_mapping >  intrusive_ptr;
			typedef void*                                   addr_t;
			
			const intrusive_ptr memory = anonymous ? map_anonymous( len )
			                                       : GetFileHandle( fd )->Map( len, off );
			
			const addr_t address = current_process().add_memory_mapping( memory.get() );
			
			return (long) address;
		}
		catch ( ... )
		{
			frame.SetErrnoFromException();
		}
		
		return (long) MAP_FAILED;
	}
	
	static int munmap( void *addr, size_t len )
	{
		SystemCallFrame frame( "munmap" );
		
		if ( len == 0 )
		{
			return frame.SetErrno( EINVAL );
		}
		
		try
		{
			current_process().remove_memory_mapping( addr );
		}
		catch ( ... )
		{
			return frame.SetErrnoFromException();
		}
		
		return 0;
	}
	
	#pragma force_active on
	
	REGISTER_SYSTEM_CALL( _lamp_mmap );
	REGISTER_SYSTEM_CALL( munmap     );
	
	#pragma force_active reset
	
}

