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
#include "Genie/FS/ResolvePathAt.hh"
#include "Genie/IO/RegularFile.hh"
#include "Genie/Process.hh"
#include "Genie/SystemCallRegistry.hh"
#include "Genie/SystemCalls.hh"


namespace Genie
{
	
	static int openat( int dirfd, const char* path, int flags, mode_t mode )
	{
		SystemCallFrame frame( "openat" );
		
		try
		{
			int fd = LowestUnusedFileDescriptor();
			
			FSTreePtr file = ResolvePathAt( dirfd, path );
			
			const bool directory = flags & O_DIRECTORY;
			
			if ( directory  &&  (flags & O_ACCMODE) != O_RDONLY )
			{
				return frame.SetErrno( EISDIR );
			}
			
			boost::shared_ptr< IOHandle > opened = directory ? file->OpenDirectory()
			                                                 : file->Open( flags, mode );
			
			const bool truncating = flags & O_TRUNC;
			const bool lazy       = flags & O_LAZY;
			
			if ( truncating  && !lazy )
			{
				if ( RegularFileHandle* file_handle = IOHandle_Cast< RegularFileHandle >( opened.get() ) )
				{
					file_handle->SetEOF( 0 );
				}
			}
			
			const bool close_on_exec = flags & O_CLOEXEC;
			
			AssignFileDescriptor( fd, opened, close_on_exec );
			
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
				return DuplicateFileDescriptor( filedes, LowestUnusedFileDescriptor( param ) );
			}
			
			FileDescriptor& descriptor = GetFileDescriptor( filedes );
			
			switch ( cmd )
			{
				case F_GETFD:
					return descriptor.closeOnExec;
				
				case F_SETFD:
					descriptor.closeOnExec = param;
					return 0;
				
				default:
					break;
			}
			
			IOHandle& handle = *descriptor.handle;
			
			StreamHandle& stream = IOHandle_Cast< StreamHandle >( handle );
			
			const OpenFlags mask = O_APPEND | O_NONBLOCK | O_LAZY;  // settable flags
			
			switch ( cmd )
			{
				case F_GETFL:
					return stream.GetFlags();
					
				case F_SETFL:
					// Current unsettable flags plus new settable flags
					stream.SetFlags( stream.GetFlags() & ~mask | param & mask );
					break;
				
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

