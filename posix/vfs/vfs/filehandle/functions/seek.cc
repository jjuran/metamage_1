/*
	seek.cc
	-------
*/

#include "vfs/filehandle/functions/seek.hh"

// POSIX
#include <fcntl.h>
#include <unistd.h>

// Extended API Set, part 2
#include "extended-api-set/part-2.h"

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
	
	
	static bool is_seekable( filehandle& that )
	{
		if ( that.methods()  &&  that.methods()->bstore_methods )
		{
			return true;
		}
		
		return that.get_flags() & O_DIRECTORY;
	}
	
	off_t seek( filehandle& that, off_t offset, int whence )
	{
		if ( !is_seekable( that ) )
		{
			p7::throw_errno( ESPIPE );
		}
		
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

