/*
	fstat.cc
	--------
*/

#include "relix/syscall/fstat.hh"

// POSIX
#include <sys/stat.h>

// Standard C
#include <string.h>

// vfs
#include "vfs/filehandle.hh"
#include "vfs/node.hh"
#include "vfs/filehandle/primitives/geteof.hh"
#include "vfs/filehandle/primitives/get_file.hh"
#include "vfs/primitives/stat.hh"

// relix-kernel
#include "relix/api/errno.hh"
#include "relix/api/get_fd_handle.hh"


namespace relix
{
	
	int fstat( int fd, struct stat* sb )
	{
		memset( (void*) sb, '\0', sizeof (struct stat) );
		
		try
		{
			vfs::filehandle& handle = get_fd_handle( fd );
			
			stat( *get_file( handle ), *sb );
			
			if ( sb->st_size == 0 )
			{
				try
				{
					sb->st_size = geteof( handle );
				}
				catch ( ... )
				{
				}
			}
		}
		catch ( ... )
		{
			return set_errno_from_exception();
		}
		
		return 0;
	}
	
}
