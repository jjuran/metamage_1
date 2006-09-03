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
#include "Genie/IO/SimpleDevice.hh"
#include "Genie/IO/RegularFile.hh"
#include "Genie/FileSignature.hh"
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
			
			unsigned char rdPerm = oflag + 1  &  FREAD;
			unsigned char wrPerm = oflag + 1  &  FWRITE;
			
			bool nonblocking = oflag & O_NONBLOCK;
			bool appending   = oflag & O_APPEND;
			// ...
			bool creating    = oflag & O_CREAT;
			bool truncating  = oflag & O_TRUNC;
			bool excluding   = oflag & O_EXCL;
			// ...
			bool resFork     = oflag & O_ResFork;
			bool resMap      = oflag & O_ResMap;
			
			std::string pathname = path;
			
			boost::shared_ptr< IOHandle > io;
			
			if ( pathname.substr( 0, 5 ) == "/dev/" )
			{
				io = GetSimpleDeviceHandle( path );
			}
			else
			{
				// assume it's a file
				FSSpec file = ResolveUnixPathname( path,
				                                   NN::Convert< N::FSDirSpec >( CurrentProcess().CurrentDirectory() ) );
				
				if ( creating )
				{
					if ( !N::FSpTestFileExists( file ) )
					{
						std::string name = NN::Convert< std::string >( file.name );
						
						FileSignature sig = PickFileSignatureForName( name );
						
						N::FSpCreate( file, sig.creator, sig.type );
					}
					else if ( excluding )
					{
						// error creating
						throw N::DupFNErr();
					}
				}
				
				NN::Owned< N::FSFileRefNum > fileH = N::FSpOpenDF( file, rdPerm | wrPerm );
				
				if ( truncating )
				{
					N::SetEOF( fileH, 0 );
				}
				else if ( appending )
				{
					N::SetFPos( fileH, fsFromLEOF, 0 );
				}
				
				io = OpenFile( fileH );
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

