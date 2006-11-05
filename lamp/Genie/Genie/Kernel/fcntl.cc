/*	========
 *	fcntl.cc
 *	========
 */

// Standard C
#include <errno.h>

// POSIX
#include "fcntl.h"

// Nitrogen / Carbon
#include "Nitrogen/OSStatus.h"

// Nitrogen Extras / Utilities
#include "Utilities/Files.h"

// Genie
#include "Genie/Devices.hh"
#include "Genie/FileSignature.hh"
#include "Genie/FileSystem/ResolvePathname.hh"
#include "Genie/IO/SimpleDevice.hh"
#include "Genie/IO/RegularFile.hh"
#include "Genie/pathnames.hh"
#include "Genie/Process.hh"
#include "Genie/SystemCallRegistry.hh"
#include "Genie/Yield.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	
	static boost::shared_ptr< RegularFileHandle > OpenFile( NN::Owned< N::FSFileRefNum > refNum )
	{
		return boost::shared_ptr< RegularFileHandle >( new RegularFileHandle( refNum ) );
	}
	
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
			
			std::string pathname = path;
			
			boost::shared_ptr< IOHandle > io;
			
			if ( pathname.substr( 0, 5 ) == "/dev/" )
			{
				io = GetSimpleDeviceHandle( path );
			}
			else
			{
				FSTreePtr file = ResolvePathname( path, CurrentProcess().CurrentWorkingDirectory() );
				
				io = file->Open( oflag, mode );
			}
			
			files[ fd ] = io;
			
			return fd;
		}
		catch ( ... )
		{
			// FIXME
			return CurrentProcess().SetErrno( EINVAL );
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

