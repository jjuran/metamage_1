/*	========
 *	fcntl.cc
 *	========
 */

// Standard C
#include <errno.h>

// POSIX
#include "fcntl.h"

// Genie
#include "Genie/FileDescriptors.hh"
#include "Genie/FileSystem/ResolvePathAt.hh"
#include "Genie/FileSystem/ResolvePathname.hh"
#include "Genie/Process.hh"
#include "Genie/SystemCallRegistry.hh"
#include "Genie/SystemCalls.hh"


namespace Genie
{
	
	static int openat( int dirfd, const char* path, int flags, mode_t mode )
	{
		SystemCallFrame frame( "openat" );
		
		Breathe();
		
		try
		{
			int fd = LowestUnusedFileDescriptor();
			
			FSTreePtr file = ResolvePathAt( dirfd, path );
			
			const bool following = (flags & O_NOFOLLOW) == 0;
			
			if ( following )
			{
				ResolveLinks_InPlace( file );
			}
			
			const bool directory = flags & O_DIRECTORY;
			
			if ( directory  &&  (flags & O_ACCMODE) != O_RDONLY )
			{
				return frame.SetErrno( EISDIR );
			}
			
			AssignFileDescriptor( fd, directory ? file->OpenDirectory()
			                                    : file->Open( flags, mode ) );
			
			return fd;
		}
		catch ( ... )
		{
			return frame.SetErrnoFromException();
		}
	}
	
	static int fcntl( int filedes, int cmd, int param )
	{
		SystemCallFrame frame( "fcntl" );
		
		try
		{
			if ( cmd == F_DUPFD )
			{
				return DuplicateFileDescriptor( filedes );
			}
			
			FileDescriptor& descriptor = GetFileDescriptor( filedes );
			
			switch ( cmd )
			{
				case F_GETFD:
					return descriptor.closeOnExec;
				
				case F_SETFD:
					descriptor.closeOnExec = param;
					return 0;
				
				/*
				case F_GETFL:
				case F_SETFL:
					break;
				*/
				
				default:
					break;
			}
		}
		catch ( ... )
		{
			return frame.SetErrnoFromException();
		}
		
		return frame.SetErrno( EINVAL );
	}
	
	#pragma force_active on
	
	REGISTER_SYSTEM_CALL( openat );
	REGISTER_SYSTEM_CALL( fcntl  );
	
	#pragma force_active reset
	
}

