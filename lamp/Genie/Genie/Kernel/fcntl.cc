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
#include "Genie/FileSystem/ResolvePathname.hh"
#include "Genie/Process.hh"
#include "Genie/SystemCallRegistry.hh"
#include "Genie/SystemCalls.hh"
#include "Genie/Yield.hh"


namespace Genie
{
	
	DECLARE_MODULE_INIT( Kernel_fcntl )
	DEFINE_MODULE_INIT( Kernel_fcntl )
	
	namespace P7 = POSeven;
	
	// FIXME:  Duplicate code
	static int LowestUnusedFrom( const FileDescriptorMap& files, int fd )
	{
		while ( files.find( fd ) != files.end() )
		{
			++fd;
		}
		
		return fd;
	}
	
	
	static int open( const char* path, int oflag, mode_t mode )
	{
		try
		{
			FileDescriptorMap& files = CurrentProcess().FileDescriptors();
			
			int fd = LowestUnusedFrom( files, 0 );
			
			FSTreePtr file = ResolvePathname( path, CurrentProcess().GetCWD() );
			
			if ( file->IsLink() )
			{
				file = file->ResolveLink();
			}
			
			boost::shared_ptr< IOHandle > io = file->Open( oflag, mode );
			
			files[ fd ] = io;
			
			return fd;
		}
		catch ( ... )
		{
			return GetErrnoFromExceptionInSystemCall();
		}
	}
	
	REGISTER_SYSTEM_CALL( open );
	
	static int DuplicateFD( FileDescriptorMap& files, int oldfd )
	{
		int newfd = LowestUnusedFrom( CurrentProcess().FileDescriptors(), 0 );
		
		files[ newfd ] = files[ oldfd ].handle;  // Clears the closeOnExec flag
		
		return newfd;
	}
	
	static int fcntl( int filedes, int cmd, int param )
	{
		FileDescriptorMap& files = CurrentProcess().FileDescriptors();
		
		switch ( cmd )
		{
			case F_DUPFD:
				return DuplicateFD( files, filedes );
			
			case F_GETFD:
				return files[ filedes ].closeOnExec;
			
			case F_SETFD:
				files[ filedes ].closeOnExec = param;
				return 0;
			
			/*
			case F_GETFL:
			case F_SETFL:
				break;
			*/
			
			default:
				break;
		}
		
		return CurrentProcess().SetErrno( EINVAL );
	}
	
	REGISTER_SYSTEM_CALL( fcntl );
	
}

