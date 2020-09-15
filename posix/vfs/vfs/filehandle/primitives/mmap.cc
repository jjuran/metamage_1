/*
	mmap.cc
	-------
*/

#include "vfs/filehandle/primitives/mmap.hh"

// POSIX
#include <fcntl.h>
#include <sys/mman.h>

// poseven
#include "poseven/types/errno_t.hh"

// vfs
#include "vfs/filehandle.hh"
#include "vfs/memory_mapping.hh"
#include "vfs/filehandle/methods/bstore_method_set.hh"
#include "vfs/filehandle/methods/filehandle_method_set.hh"
#include "vfs/filehandle/methods/general_method_set.hh"
#include "vfs/mmap/functions/map_file.hh"


namespace vfs
{
	
	namespace p7 = poseven;
	
	
	static inline bool needs_write_access( int prot, int flags )
	{
		return (prot & PROT_WRITE)  &&  (flags & MAP_SHARED);
	}
	
	memory_mapping_ptr mmap( filehandle* that, size_t length, int prot, int flags, off_t offset )
	{
		const int accmode = that->get_flags() & O_ACCMODE;
		
		if ( accmode == O_RDWR )
		{
			// reading and writing both allowed
		}
		else if ( accmode == O_RDONLY  &&  !needs_write_access( prot, flags ) )
		{
			// read-only, but write access isn't required
		}
		else
		{
			// no read access, or read-only with PROT_WRITE / MAP_SHARED
			
			p7::throw_errno( EACCES );
		}
		
		if ( const filehandle_method_set* methods = that->methods() )
		{
			if ( const general_method_set* general_methods = methods->general_methods )
			{
				if ( general_methods->mmap )
				{
					return general_methods->mmap( that, length, prot, flags, offset );
				}
			}
			
			if ( methods->bstore_methods )
			{
				// Generic implementation for any regular file
				
				return map_file( *that, length, prot, flags, offset );
			}
		}
		
		p7::throw_errno( ENODEV );
		
		// Not reached
		return memory_mapping_ptr();
	}
	
}
