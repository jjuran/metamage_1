/*
	nonblocking.hh
	--------------
*/

#ifndef VFS_FILEHANDLE_FUNCTIONS_NONBLOCKING_HH
#define VFS_FILEHANDLE_FUNCTIONS_NONBLOCKING_HH

// POSIX
#include <fcntl.h>

// vfs
#include "vfs/filehandle.hh"


namespace vfs
{
	
	inline bool is_nonblocking( const filehandle& that )
	{
		return that.get_flags() & O_NONBLOCK;
	}
	
	inline void set_nonblocking( filehandle& that )
	{
		that.set_flags( that.get_flags() |  O_NONBLOCK );
	}
	
	inline void clear_nonblocking( filehandle& that )
	{
		that.set_flags( that.get_flags() & ~O_NONBLOCK );
	}
	
}

#endif

