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
#include "Genie/FileSignature.hh"
#include "Genie/FileSystem/ResolvePathname.hh"
#include "Genie/FileSystem/StatFile.hh"
#include "Genie/pathnames.hh"
#include "Genie/Process.hh"
#include "Genie/SystemCallRegistry.hh"
#include "Genie/Yield.hh"


namespace Nitrogen
{
	
	static void FSpSetFInfo( const FSSpec& file, const FInfo& info )
	{
		ThrowOSStatus( ::FSpSetFInfo( &file, &info ) );
	}
	
	static void FSpSetFLock( const FSSpec& file )
	{
		ThrowOSStatus( ::FSpSetFLock( &file ) );
	}
	
	static void FSpRstFLock( const FSSpec& file )
	{
		ThrowOSStatus( ::FSpRstFLock( &file ) );
	}
	
}

namespace Genie
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	namespace P7 = POSeven;
	
	static bool TypeIsExecutable( OSType type )
	{
		switch ( type )
		{
			case 'Wish':
			case 'MPST':
			case 'APPL':
				return true;
			
			default:
				break;
		}
		
		return false;
	}
	
	static mode_t FileWXModeBits( const HFileInfo& hFileInfo )
	{
		bool locked = hFileInfo.ioFlAttrib & kioFlAttribLockedMask;
		bool writable = !locked;
		
		const FInfo& info = hFileInfo.ioFlFndrInfo;
		OSType type = info.fdType;
		bool executable = TypeIsExecutable( type ) || type == 'TEXT' && (info.fdFlags & kIsShared || info.fdCreator == 'Poof');
		
		return ( writable ? S_IWUSR : 0 ) | ( executable ? S_IXUSR : 0 );
	}
	
	static mode_t GetItemMode( const HFileInfo& hFileInfo )
	{
		bool isDir = hFileInfo.ioFlAttrib & kioFlAttribDirMask;
		
		mode_t mode = S_IRUSR | ( isDir ? S_IFDIR | S_IWUSR | S_IXUSR
		                                : S_IFREG | FileWXModeBits( hFileInfo ) );
		
		return mode;
	}
	
	static void ChangeFileMode( const FSSpec& file, mode_t new_mode )
	{
		CInfoPBRec paramBlock;
		
		N::FSpGetCatInfo( file, paramBlock );
		
		HFileInfo& hFileInfo = paramBlock.hFileInfo;
		
		bool isDir = hFileInfo.ioFlAttrib & kioFlAttribDirMask;
		
		if ( isDir )
		{
			// Can't change permissions on directories
			throw P7::Errno( EPERM );
		}
		
		mode_t current_mode = GetItemMode( hFileInfo );
		
		mode_t changed_bits = new_mode ^ current_mode;
		
		if ( changed_bits & S_IRUSR )
		{
			// Can't make anything unreadable
			throw P7::Errno( EPERM );
		}
		
		if ( changed_bits & S_IXUSR )
		{
			FInfo& info = hFileInfo.ioFlFndrInfo;
			
			if ( info.fdType != 'TEXT' )
			{
				// Can't change executability of non-scripts
				// (e.g. don't remove Shared bit on apps)
				throw P7::Errno( EPERM );
			}
			
			bool x_bit = new_mode & S_IXUSR;
			
			info.fdFlags = (info.fdFlags & ~kIsShared) | (kIsShared * x_bit);
			info.fdCreator = x_bit ? 'Poof' : TextFileCreator();
			
			N::FSpSetFInfo( file, info );
		}
		
		if ( changed_bits & S_IWUSR )
		{
			if ( new_mode & S_IWUSR )
			{
				// writeable: reset the lock
				N::FSpRstFLock( file );
			}
			else
			{
				// not writeable: set the lock
				N::FSpSetFLock( file );
			}
		}
	}
	
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

