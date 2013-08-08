/*
	pread.cc
	--------
*/

#include "vfs/filehandle/primitives/pread.hh"

// poseven
#include "poseven/types/errno_t.hh"

// vfs
#include "vfs/filehandle.hh"
#include "vfs/filehandle/methods/bstore_method_set.hh"


namespace vfs
{
	
	namespace p7 = poseven;
	
	
	ssize_t pread( filehandle& that, char* buffer, size_t n, off_t offset )
	{
		const vfs::bstore_method_set& bstore_methods = that.bstore_methods();
		
		if ( bstore_methods.pread == NULL )
		{
			p7::throw_errno( EINVAL );
		}
		
		return bstore_methods.pread( &that, buffer, n, offset );
	}
	
}

