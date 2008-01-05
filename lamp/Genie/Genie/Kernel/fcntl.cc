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
#include "Genie/FileSystem/ResolvePathname.hh"
#include "Genie/Process.hh"
#include "Genie/SystemCallRegistry.hh"
#include "Genie/SystemCalls.hh"
#include "Genie/Yield.hh"


namespace Genie
{
	
	DECLARE_MODULE_INIT( Kernel_fcntl )
	DEFINE_MODULE_INIT( Kernel_fcntl )
	
	
	static int open( const char* path, int oflag, mode_t mode )
	{
		SystemCallFrame frame( "open" );
		
		Breathe();
		
		try
		{
			int fd = LowestUnusedFileDescriptor();
			
			FSTreePtr file = ResolvePathname( path, frame.Caller().GetCWD() );
			
			ResolveLinks_InPlace( file );
			
			bool is_dir = file->IsDirectory();
			
			if ( is_dir  &&  oflag != O_RDONLY )
			{
				return frame.SetErrno( EISDIR );
			}
			
			AssignFileDescriptor( fd, is_dir ? file->OpenDirectory()
			                                 : file->Open( oflag, mode ) );
			
			return fd;
		}
		catch ( ... )
		{
			return frame.SetErrnoFromException();
		}
	}
	
	REGISTER_SYSTEM_CALL( open );
	
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
	
	REGISTER_SYSTEM_CALL( fcntl );
	
}

