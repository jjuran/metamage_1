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
#include "vfs/file_descriptor.hh"
#include "vfs/functions/resolve_links_in_place.hh"

// Genie
#include "Genie/current_process.hh"
#include "Genie/FileDescriptors.hh"
#include "Genie/FS/file-tests.hh"
#include "Genie/FS/ResolvePathAt.hh"
#include "Genie/FS/open.hh"
#include "Genie/FS/opendir.hh"
#include "Genie/IO/Stream.hh"
#include "Genie/SystemCallRegistry.hh"


#ifndef O_DIRECTORY
#define O_DIRECTORY  0
#endif

#ifndef O_CLOEXEC
#define O_CLOEXEC  0
#endif

#ifndef F_DUPFD_CLOEXEC
#define F_DUPFD_CLOEXEC  F_DUPFD
#endif


namespace Genie
{
	
	static int openat( int dirfd, const char* path, int flags, mode_t mode )
	{
		try
		{
			int fd = LowestUnusedFileDescriptor();
			
			FSTreePtr file = ResolvePathAt( dirfd, path );
			
			const bool excluding = flags & O_EXCL;
			
			if ( excluding  &&  exists( file ) )
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
			else if ( is_symlink( file ) )
			{
				return set_errno( ELOOP );
			}
			
			if ( directory  &&  !is_directory( file ) )
			{
				return set_errno( exists( file ) ? ENOTDIR : ENOENT );
			}
			
			IOPtr opened = directory ? opendir( file.get()              )
			                         : open   ( file.get(), flags, mode );
			
			const bool close_on_exec = flags & O_CLOEXEC;
			
			assign_file_descriptor( fd, *opened, close_on_exec );
			
			return fd;
		}
		catch ( ... )
		{
			return set_errno_from_exception();
		}
	}
	
	static int fcntl( int filedes, int cmd, int param )
	{
		try
		{
			if ( const bool dup = (cmd | O_CLOEXEC) == F_DUPFD_CLOEXEC )
			{
				const bool close_on_exec = cmd == F_DUPFD_CLOEXEC;
				
				return DuplicateFileDescriptor( filedes,
				                                LowestUnusedFileDescriptor( param ),
				                                close_on_exec );
			}
			
			vfs::file_descriptor& descriptor = GetFileDescriptor( filedes );
			
			switch ( cmd )
			{
				case F_GETFD:
					return descriptor.will_close_on_exec();
				
				case F_SETFD:
					descriptor.set_to_close_on_exec( param );
					return 0;
				
				default:
					break;
			}
			
			IOHandle& handle = *descriptor.handle;
			
			StreamHandle& stream = IOHandle_Cast< StreamHandle >( handle );
			
			const int mask = O_APPEND | O_NONBLOCK;  // settable flags
			
			switch ( cmd )
			{
				case F_GETFL:
					return stream.GetFlags();
					
				case F_SETFL:
					// Current unsettable flags plus new settable flags
					stream.SetFlags( (stream.GetFlags() & ~mask) | (param & mask) );
					return 0;
				
				default:
					break;
			}
		}
		catch ( ... )
		{
			return set_errno_from_exception();
		}
		
		return set_errno( EINVAL );
	}
	
	#pragma force_active on
	
	REGISTER_SYSTEM_CALL( openat );
	REGISTER_SYSTEM_CALL( fcntl  );
	
	#pragma force_active reset
	
}

