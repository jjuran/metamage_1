/*	=========
 *	dirent.cc
 *	=========
 */

// Standard C
#include <errno.h>

// POSIX
#include "dirent.h"

// Nitrogen
#include "Nitrogen/OSStatus.h"

// Genie
#include "Genie/IO/Directory.hh"
#include "Genie/Process.hh"
#include "Genie/SystemCallRegistry.hh"
#include "Genie/Yield.hh"


namespace Genie
{

	namespace N = Nitrogen;
	
	
	class PathnameComponentIterator
	{
		private:
			const char* pathname_begin;
			const char* pathname_end;
			const char* p;
			const char* q;
		
		public:
			PathnameComponentIterator( const std::string& pathname ) : pathname_begin( pathname.c_str() ),
			                                                           pathname_end  ( pathname_begin + pathname.size() ),
			                                                           p             ( pathname_begin ),
			                                                           q             ( p )
			{
				Scan();
			}
			
			void Scan()
			{
				q = std::strchr( p, '/' );
				
				if ( q == NULL )
				{
					q = pathname_end;
				}
			}
			
			bool Done() const
			{
				return p == pathname_end;
			}
			
			void Advance()
			{
				do
				{
					if ( q == pathname_end )
					{
						p = q;
						break;
					}
					
					p = q + 1;
					Scan();
				}
				while ( p == q );  // Skip empty segments
			}
			
			std::string Get() const
			{
				ASSERT( p != NULL );
				ASSERT( q != NULL );
				
				ASSERT( p <= q );
				
				ASSERT( p >= pathname_begin );
				ASSERT( q <= pathname_end   );
				
				return std::string( p, q );
			}
	};
	
	static FSTreePtr ResolvePathname( const std::string& pathname, FSTreePtr current )
	{
		PathnameComponentIterator path( pathname );
		
		if ( path.Get().empty() )
		{
			current = FSRoot();
			
			path.Advance();
		}
		
		FSTreePtr result = current;
		
		while ( !path.Done() )
		{
			result = result->Lookup( path.Get() );
			
			path.Advance();
		}
		
		return result;
	}
	
	static boost::shared_ptr< DirHandle > OpenDir( const std::string& pathname )
	{
		FSTreePtr current( CurrentProcess().CurrentWorkingDirectory() );
		
		return boost::shared_ptr< DirHandle >( new DirHandle( ResolvePathname( pathname, current ) ) );
	}
	
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
			files[ fd ] = boost::shared_ptr< IOHandle >( OpenDir( pathname ) );
			
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
			DirHandle& dir = IOHandle_Cast< DirHandle >( *files[ fd ].handle );
			
			files.erase( fd );
			
			return 0;
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
			DirHandle& dir = IOHandle_Cast< DirHandle >( *files[ fd ].handle );
			
			return dir.ReadDir();
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
			DirHandle& dir = IOHandle_Cast< DirHandle >( *files[ fd ].handle );
			
			dir.RewindDir();
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
			DirHandle& dir = IOHandle_Cast< DirHandle >( *files[ fd ].handle );
			
			dir.SeekDir( offset );
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
			DirHandle& dir = IOHandle_Cast< DirHandle >( *files[ fd ].handle );
			
			return dir.TellDir();
		}
		catch ( ... )
		{
			
		}
		
		return CurrentProcess().SetErrno( EINVAL );
	}
	
	REGISTER_SYSTEM_CALL( telldir );
	
}

