/*	=========
 *	dirent.cc
 *	=========
 */

// POSIX
#include "dirent.h"
#include <fcntl.h>
#include <errno.h>

// Extended API Set, part 2
#include "extended-api-set/part-2.h"

// vfs
#include "vfs/filehandle.hh"
#include "vfs/filehandle/primitives/read.hh"

// relix
#include "relix/api/get_fd_handle.hh"

// Genie
#include "Genie/current_process.hh"
#include "Genie/SystemCallRegistry.hh"


int getdents( unsigned fd, struct dirent* dirp, unsigned int count )
{
	using namespace Genie;
	
	try
	{
		vfs::filehandle& dir = relix::get_fd_handle( fd );
		
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

#pragma force_active on

REGISTER_SYSTEM_CALL( getdents );

#pragma force_active reset

