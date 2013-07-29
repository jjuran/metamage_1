/*
	open.cc
	-------
*/

#include "vfs/primitives/open.hh"

// POSIX
#include <fcntl.h>

// poseven
#include "poseven/types/errno_t.hh"

// vfs
#include "vfs/filehandle.hh"
#include "vfs/node.hh"
#include "vfs/filehandle/functions/truncate.hh"
#include "vfs/methods/data_method_set.hh"
#include "vfs/methods/node_method_set.hh"


namespace vfs
{
	
	namespace p7 = poseven;
	
	
	filehandle_ptr open( const node* that, int flags, mode_t mode )
	{
		const node_method_set* methods = that->methods();
		
		const data_method_set* data_methods;
		
		if ( methods  &&  (data_methods = methods->data_methods) )
		{
			if ( data_methods->open )
			{
				filehandle_ptr result = data_methods->open( that, flags, mode );
				
				if ( flags & O_TRUNC )
				{
					truncate( result.get() );
				}
				
				return result;
			}
		}
		
		if ( (flags & (O_CREAT|O_EXCL)) == (O_CREAT|O_EXCL) )
		{
			p7::throw_errno( EPERM );
		}
		
		throw p7::errno_t( flags & O_CREAT ? EPERM : ENOENT );
	}
	
}

