/*
	getdents.cc
	-----------
*/

#include "relix/syscall/getdents.hh"

#ifdef __APPLE__
#include <sys/types.h>
#endif

// POSIX
#include <dirent.h>
#include <fcntl.h>
#include <errno.h>

// Extended API Set, part 2
#include "extended-api-set/part-2.h"

// vfs
#include "vfs/filehandle.hh"
#include "vfs/filehandle/primitives/read.hh"

// relix
#include "relix/api/errno.hh"
#include "relix/api/get_fd_handle.hh"


namespace relix
{
	
	int getdents( unsigned fd, struct dirent* dirp, unsigned int count )
	{
		try
		{
			vfs::filehandle& dir = get_fd_handle( fd );
			
			if ( (dir.get_flags() & O_DIRECTORY) == 0 )
			{
				return set_errno( ENOTDIR );
			}
			
			if ( count < sizeof (dirent) )
			{
				return set_errno( EINVAL );
			}
			
			return read( dir, (char*) dirp, sizeof (dirent) );
		}
		catch ( ... )
		{
			return set_errno_from_exception();
		}
	}
	
}
