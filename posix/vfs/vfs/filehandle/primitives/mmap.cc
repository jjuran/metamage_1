/*
	mmap.cc
	-------
*/

#include "vfs/filehandle/primitives/mmap.hh"

// vfs
#include "vfs/filehandle.hh"
#include "vfs/memory_mapping.hh"
#include "vfs/filehandle/methods/filehandle_method_set.hh"
#include "vfs/filehandle/methods/general_method_set.hh"


namespace vfs
{
	
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
		}
		
		return that->Map( length, prot, flags, offset );
	}
	
}

