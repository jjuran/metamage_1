/*	========
 *	fcntl.cc
 *	========
 */

// Standard C
#include <errno.h>

// POSIX
#include "fcntl.h"

// vfs
#include "vfs/node.hh"
#include "vfs/filehandle.hh"
#include "vfs/functions/file-tests.hh"
#include "vfs/functions/resolve_links_in_place.hh"
#include "vfs/primitives/open.hh"
#include "vfs/primitives/opendir.hh"

// relix-kernel
#include "relix/api/assign_fd.hh"
#include "relix/api/first_free_fd.hh"
#include "relix/syscall/fcntl.hh"

// Genie
#include "Genie/current_process.hh"
#include "Genie/FS/ResolvePathAt.hh"
#include "Genie/SystemCallRegistry.hh"


#ifndef O_DIRECTORY
#define O_DIRECTORY  0
#endif

#ifndef O_CLOEXEC
#define O_CLOEXEC  0
#endif


namespace Genie
{
	
	static int openat( int dirfd, const char* path, int flags, mode_t mode )
	{
		try
		{
			int fd = relix::first_free_fd();
			
			FSTreePtr file = ResolvePathAt( dirfd, path );
			
			const bool excluding = flags & O_EXCL;
			
			if ( excluding  &&  exists( *file ) )
			{
				return set_errno( EEXIST );
			}
			
			const bool directory = flags & O_DIRECTORY;
			
			if ( directory  &&  (flags & O_ACCMODE) != O_RDONLY )
			{
				return set_errno( EISDIR );
			}
			
			if ( const bool following = (flags & O_NOFOLLOW) == 0 )
			{
				vfs::resolve_links_in_place( file );
			}
			else if ( is_symlink( *file ) )
			{
				return set_errno( ELOOP );
			}
			
			if ( directory  &&  !is_directory( *file ) )
			{
				return set_errno( exists( *file ) ? ENOTDIR : ENOENT );
			}
			
			vfs::filehandle_ptr opened = directory ? opendir( *file              )
			                                       : open   ( *file, flags, mode );
			
			const bool close_on_exec = flags & O_CLOEXEC;
			
			relix::assign_fd( fd, *opened, close_on_exec );
			
			return fd;
		}
		catch ( ... )
		{
			return set_errno_from_exception();
		}
	}
	
	
	using relix::fcntl;
	
	
	#pragma force_active on
	
	REGISTER_SYSTEM_CALL( openat );
	REGISTER_SYSTEM_CALL( fcntl  );
	
	#pragma force_active reset
	
}

