/*	=========
 *	dirent.cc
 *	=========
 */

// POSIX
#include "dirent.h"

// Nitrogen
#include "Nitrogen/MacErrors.h"

// Genie
#include "Genie/FileDescriptors.hh"
#include "Genie/FileSystem/ResolvePathname.hh"
#include "Genie/IO/Directory.hh"
#include "Genie/Process.hh"
#include "Genie/SystemCallRegistry.hh"
#include "Genie/SystemCalls.hh"
#include "Genie/Yield.hh"


#undef dirfd


namespace Genie
{
	
	DECLARE_MODULE_INIT( Kernel_dirent )
	DEFINE_MODULE_INIT( Kernel_dirent )
	
	namespace N = Nitrogen;
	
	
	static boost::shared_ptr< IOHandle > OpenDir( const std::string& pathname )
	{
		FSTreePtr current( CurrentProcess().GetCWD() );
		
		FSTreePtr dir = ResolvePathname( pathname, current );
		
		if ( dir->IsLink() )
		{
			dir = dir->ResolveLink();
		}
		
		return boost::shared_ptr< IOHandle >( new DirHandle( dir ) );
	}
	
	/*
	struct dirent
	{
		ino_t d_ino;
		char  d_name[256];
	};
	*/
	
	static DIR* opendir( const char* pathname )
	{
		SystemCallFrame frame( "opendir" );
		
		Breathe();
		
		try
		{
			int fd = LowestUnusedFileDescriptor( 3 );
			
			AssignFileDescriptor( fd, OpenDir( pathname ) );
			
			return reinterpret_cast< DIR* >( fd );
		}
		catch ( ... )
		{
			(void) frame.SetErrnoFromException();
		}
		
		return NULL;
	}
	
	REGISTER_SYSTEM_CALL( opendir );
	
	static int closedir( DIR* dir )
	{
		SystemCallFrame frame( "closedir" );
		
		int fd = reinterpret_cast< int >( dir );
		
		try
		{
			DirHandle& dir = GetFileHandleWithCast< DirHandle >( fd );
			
			CloseFileDescriptor( fd );
		}
		catch ( ... )
		{
			return frame.SetErrnoFromException();
		}
		
		return 0;
	}
	
	REGISTER_SYSTEM_CALL( closedir );
	
	static const dirent* readdir( DIR* dir )
	{
		SystemCallFrame frame( "readdir" );
		
		Breathe();
		
		int fd = reinterpret_cast< int >( dir );
		
		try
		{
			DirHandle& dir = GetFileHandleWithCast< DirHandle >( fd );
			
			return dir.ReadDir();
		}
		catch ( const N::OSStatus& err )
		{
			if ( err == fnfErr )
			{
				// End of iteration -- not an error, so clear errno
				frame.SetErrno( 0 );
			}
			else
			{
				(void) frame.SetErrnoFromException();
			}
		}
		catch ( ... )
		{
			(void) frame.SetErrnoFromException();
		}
		
		return NULL;
	}
	
	REGISTER_SYSTEM_CALL( readdir );
	
	static void rewinddir( DIR* dir )
	{
		SystemCallFrame frame( "rewinddir" );
		
		int fd = reinterpret_cast< int >( dir );
		
		try
		{
			DirHandle& dir = GetFileHandleWithCast< DirHandle >( fd );
			
			dir.RewindDir();
		}
		catch ( ... )
		{
			(void) frame.SetErrnoFromException();
		}
	}
	
	REGISTER_SYSTEM_CALL( rewinddir );
	
	static void seekdir( DIR* dir, off_t offset )
	{
		SystemCallFrame frame( "seekdir" );
		
		int fd = reinterpret_cast< int >( dir );
		
		try
		{
			DirHandle& dir = GetFileHandleWithCast< DirHandle >( fd );
			
			dir.SeekDir( offset );
		}
		catch ( ... )
		{
			(void) frame.SetErrnoFromException();
		}
	}
	
	REGISTER_SYSTEM_CALL( seekdir );
	
	static off_t telldir( DIR* dir )
	{
		SystemCallFrame frame( "telldir" );
		
		int fd = reinterpret_cast< int >( dir );
		
		try
		{
			DirHandle& dir = GetFileHandleWithCast< DirHandle >( fd );
			
			return dir.TellDir();
		}
		catch ( ... )
		{
			return frame.SetErrnoFromException();
		}
	}
	
	REGISTER_SYSTEM_CALL( telldir );
	
	static int dirfd( DIR* dir )
	{
		SystemCallFrame frame( "dirfd" );
		
		int fd = reinterpret_cast< int >( dir );
		
		try
		{
			DirHandle& dir = GetFileHandleWithCast< DirHandle >( fd );
			
			return fd;
		}
		catch ( ... )
		{
			return frame.SetErrnoFromException();
		}
	}
	
	REGISTER_SYSTEM_CALL( dirfd );
	
}

