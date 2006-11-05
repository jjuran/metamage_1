/*	=======
 *	stat.cc
 *	=======
 */

// Universal Interfaces
#include <OSUtils.h>

// Standard C
#include <errno.h>
#include <string.h>

// Standard C++
#include <string>

// POSIX
#include "stdlib.h"
#include "sys/stat.h"

// Nitrogen
#include "Nitrogen/OSStatus.h"

// POSeven
#include "POSeven/Errno.hh"

// Nitrogen Extras / Utilities
#include "Utilities/Files.h"

// Kerosene/Common
#include "KEnvironment.hh"
#include "SystemCalls.hh"

// Genie
#include "Genie/IO/File.hh"
#include "Genie/FileSystem/ResolvePathname.hh"
#include "Genie/FileSystem/StatFile.hh"
#include "Genie/pathnames.hh"
#include "Genie/Process.hh"
#include "Genie/SystemCallRegistry.hh"
#include "Genie/Yield.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	namespace P7 = POSeven;
	
	static void StatCharacterDevice( struct stat* sb )
	{
		StatGeneric( sb );
	}
	
	static int chmod_file( const char* path, mode_t mode )
	{
		std::string pathname = path;
		
		if ( pathname.substr( 0, 5 ) == "/dev/" )
		{
			
		}
		else
		{
			// assume it's a file
			
			try
			{
				N::FSDirSpec current = NN::Convert< N::FSDirSpec >( CurrentProcess().CurrentDirectory() );
				
				FSSpec file = ResolveUnixPathname( path, current );
				
				ChangeFileMode( file, mode );
				
				return 0;
			}
			catch ( const P7::Errno& err )
			{
				return CurrentProcess().SetErrno( err );
			}
			catch ( const N::FNFErr& err )
			{
				return CurrentProcess().SetErrno( ENOENT );
			}
			catch ( const N::OSStatus& err )
			{
				
			}
		}
		
		return CurrentProcess().SetErrno( EINVAL );
	}
	
	static int stat_file( const char* path, struct stat* sb )
	{
		std::memset( (void*)sb, '\0', sizeof (struct stat) );
		
		std::string pathname = path;
		
		if ( pathname.substr( 0, 5 ) == "/dev/" )
		{
			
		}
		else
		{
			// assume it's a file
			
			try
			{
				FSTreePtr current = CurrentProcess().CurrentWorkingDirectory();
				
				FSTreePtr file = ResolvePathname( path, current );
				
				file->Stat( *sb );
				
				return 0;
			}
			catch ( const N::FNFErr& err )
			{
				return CurrentProcess().SetErrno( ENOENT );
			}
			catch ( const N::OSStatus& err )
			{
				
			}
		}
		
		return CurrentProcess().SetErrno( EINVAL );
	}
	
	static int chmod( const char* path, mode_t mode )
	{
		return chmod_file( path, mode );
	}
	
	REGISTER_SYSTEM_CALL( chmod );
	
	static int lstat( const char* path, struct stat* sb )
	{
		return stat_file( path, sb );
	}
	
	REGISTER_SYSTEM_CALL( lstat );
	
	static int stat( const char* path, struct stat* sb )
	{
		return stat_file( path, sb );  // FIXME:  Resolve symlinks
	}
	
	REGISTER_SYSTEM_CALL( stat );
	
	static int fstat( int fd, struct stat* sb )
	{
		FileDescriptorMap& files = CurrentProcess().FileDescriptors();
		
		try
		{
			FileHandle& fh = IOHandle_Cast< FileHandle >( *files[ fd ].handle );
			
			fh.Stat( sb );
			
			return 0;
		}
		catch ( ... )
		{
			// Convert the exception
			return CurrentProcess().SetErrno( EINVAL );
		}
		
		return 0;
	}
	
	REGISTER_SYSTEM_CALL( fstat );
	
}

