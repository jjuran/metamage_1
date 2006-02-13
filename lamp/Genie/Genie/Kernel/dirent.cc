/*	=========
 *	dirent.cc
 *	=========
 */

// Standard C
#include <errno.h>

// POSIX
#include "dirent.h"

// Nitrogen / Carbon
#include "Nitrogen/Files.h"
#include "Nitrogen/OSStatus.h"

// Genie
#include "Genie/DirHandle.hh"
#include "Genie/pathnames.hh"
#include "Genie/Process.hh"
#include "Genie/SystemCallRegistry.hh"
#include "Genie/Yield.hh"


namespace Genie
{

	namespace N = Nitrogen;
	
	
	/*
	struct dirent
	{
		ino_t d_ino;
		char  d_name[256];
	};
	*/
	
	// FIXME:  Duplicate code
	static int LowestUnusedFrom( const FileDescriptorMap& files, int fd )
	{
		while ( files.find( fd ) != files.end() )
		{
			++fd;
		}
		
		return fd;
	}
	
	static DIR* opendir( const char* pathname )
	{
		Yield();
		
		FileDescriptorMap& files = CurrentProcess().FileDescriptors();
		
		int fd = LowestUnusedFrom( files, 0 );
		
		try
		{
			N::FSDirSpec current = NN::Convert< N::FSDirSpec >( CurrentProcess().CurrentDirectory() );
			
			FSSpec spec = ResolveUnixPathname( pathname, current );
			
			N::FSDirSpec dir = NN::Convert< N::FSDirSpec >( spec );
			
			IORef ref = OpenDir( dir );
			
			files[ fd ] = ref;
			
			return reinterpret_cast< DIR* >( fd );
		}
		catch ( N::FNFErr& )
		{
			CurrentProcess().SetErrno( ENOENT );
		}
		catch ( ... )
		{
			CurrentProcess().SetErrno( EINVAL );
		}
		
		return NULL;
	}
	
	REGISTER_SYSTEM_CALL( opendir );
	
	static int closedir( DIR* dir )
	{
		int fd = reinterpret_cast< int >( dir );
		
		FileDescriptorMap& files = CurrentProcess().FileDescriptors();
		
		try
		{
			// FIXME:  Verify dirhandle exists first
			if ( files[ fd ].handle.IsType( kDirectoryIterator ) )
			{
				files.erase( fd );
				
				return 0;
			}
			else
			{
				return CurrentProcess().SetErrno( ENOTDIR );
			}
			
		}
		catch ( ... )
		{
			
		}
		
		return CurrentProcess().SetErrno( EINVAL );
	}
	
	REGISTER_SYSTEM_CALL( closedir );
	
	static const dirent* readdir( DIR* dir )
	{
		int fd = reinterpret_cast< int >( dir );
		
		FileDescriptorMap& files = CurrentProcess().FileDescriptors();
		
		try
		{
			// FIXME:  Verify dirhandle exists first
			if ( files[ fd ].handle.IsType( kDirectoryIterator ) )
			{
				DirHandle& dir = IORef_Cast< DirHandle >( files[ fd ].handle );
				
				return dir.ReadDir();
			}
			
		}
		catch ( const N::FNFErr& err )
		{
			CurrentProcess().SetErrno( 0 );
		}
		catch ( ... )
		{
			CurrentProcess().SetErrno( EINVAL );
		}
		
		return NULL;
	}
	
	REGISTER_SYSTEM_CALL( readdir );
	
	static void rewinddir( DIR* dir )
	{
		int fd = reinterpret_cast< int >( dir );
		
		FileDescriptorMap& files = CurrentProcess().FileDescriptors();
		
		try
		{
			// FIXME:  Verify dirhandle exists first
			if ( files[ fd ].handle.IsType( kDirectoryIterator ) )
			{
				DirHandle& dir = IORef_Cast< DirHandle >( files[ fd ].handle );
				
				dir.RewindDir();
			}
			else
			{
				CurrentProcess().SetErrno( ENOTDIR );
			}
			
		}
		catch ( ... )
		{
			
		}
		
		CurrentProcess().SetErrno( EINVAL );
	}
	
	REGISTER_SYSTEM_CALL( rewinddir );
	
	static void seekdir( DIR* dir, off_t offset )
	{
		int fd = reinterpret_cast< int >( dir );
		
		FileDescriptorMap& files = CurrentProcess().FileDescriptors();
		
		try
		{
			// FIXME:  Verify dirhandle exists first
			if ( files[ fd ].handle.IsType( kDirectoryIterator ) )
			{
				DirHandle& dir = IORef_Cast< DirHandle >( files[ fd ].handle );
				
				dir.SeekDir( offset );
			}
			else
			{
				CurrentProcess().SetErrno( ENOTDIR );
			}
			
		}
		catch ( ... )
		{
			
		}
		
		CurrentProcess().SetErrno( EINVAL );
	}
	
	REGISTER_SYSTEM_CALL( seekdir );
	
	static off_t telldir( DIR* dir )
	{
		int fd = reinterpret_cast< int >( dir );
		
		FileDescriptorMap& files = CurrentProcess().FileDescriptors();
		
		try
		{
			// FIXME:  Verify dirhandle exists first
			if ( files[ fd ].handle.IsType( kDirectoryIterator ) )
			{
				DirHandle& dir = IORef_Cast< DirHandle >( files[ fd ].handle );
				
				return dir.TellDir();
			}
			else
			{
				return CurrentProcess().SetErrno( ENOTDIR );
			}
			
		}
		catch ( ... )
		{
			
		}
		
		return CurrentProcess().SetErrno( EINVAL );
	}
	
	REGISTER_SYSTEM_CALL( telldir );
	
}

