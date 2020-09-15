/*
	fsync.cc
	--------
*/

#include "vfs/filehandle/primitives/fsync.hh"

// Standard C
#include <errno.h>

// poseven
#include "poseven/types/errno_t.hh"

// vfs
#include "vfs/filehandle.hh"
#include "vfs/filehandle/methods/bstore_method_set.hh"
#include "vfs/filehandle/methods/filehandle_method_set.hh"


namespace vfs
{
	
	namespace p7 = poseven;
	
	
	void fsync( filehandle& that )
	{
		if ( const filehandle_method_set* methods = that.methods() )
		{
			if ( const bstore_method_set* bstore_methods = methods->bstore_methods )
			{
				if ( bstore_methods->fsync )
				{
					bstore_methods->fsync( &that );
				}
				
				// Always return for regular files
				
				return;
			}
		}
		
		// Always throw for non-regular files
		
		p7::throw_errno( EINVAL );
	}
	
}
