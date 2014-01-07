/*
	mmap.cc
	-------
*/

#include "vfs/filehandle/primitives/mmap.hh"

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
	
	
	memory_mapping_ptr mmap( filehandle* that, size_t length, int prot, int flags, off_t offset )
	{
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

