/*	========
 *	fcntl.cc
 *	========
 */

// Standard C
#include <errno.h>

// POSIX
#include "fcntl.h"

// POSeven
#include "Errno.hh"

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
			
			FSTreePtr file = ResolvePathname( path, CurrentProcess().GetCWD() );
			
			if ( file->IsLink() )
			{
				file = file->ResolveLink();
			}
			
			AssignFileDescriptor( fd, file->Open( oflag, mode ) );
			
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
			switch ( cmd )
			{
				case F_DUPFD:
					return DuplicateFileDescriptor( filedes );
				
				case F_GETFD:
					return GetFileDescriptor( filedes ).closeOnExec;
				
				case F_SETFD:
					GetFileDescriptor( filedes ).closeOnExec = param;
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

