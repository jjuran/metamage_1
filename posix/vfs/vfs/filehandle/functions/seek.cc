/*
	seek.cc
	-------
*/

#include "vfs/filehandle/functions/seek.hh"

// POSIX
#include <unistd.h>

// poseven
#include "poseven/types/errno_t.hh"

// vfs
#include "vfs/filehandle.hh"
#include "vfs/filehandle/methods/filehandle_method_set.hh"
#include "vfs/filehandle/primitives/geteof.hh"
#include "vfs/filehandle/primitives/seteof.hh"


namespace vfs
{
	
	namespace p7 = poseven;
	
	
	off_t seek( filehandle& that, off_t offset, int whence )
	{
		// throws EISDIR or ESPIPE if not a regular file
		that.bstore_methods();
		
		off_t base = 0;
		
		switch ( whence )
		{
			case SEEK_SET:
			//	base = 0;
				break;
			
			case SEEK_CUR:
				base = that.get_mark();
				break;
			
			case SEEK_END:
				base = geteof( that );
				break;
			
			default:
				p7::throw_errno( EINVAL );
		}
		
		return that.set_mark( base + offset );
	}
	
}

