/*	=======
 *	stat.cc
 *	=======
 */

// Standard C/C++
#include <cstring>

// POSIX
//#include "stdlib.h"
#include "sys/stat.h"

// vfs
#include "vfs/primitives/chmod.hh"
#include "vfs/primitives/geteof.hh"

// Genie
#include "Genie/current_process.hh"
#include "Genie/FileDescriptors.hh"
#include "Genie/FS/file-tests.hh"
#include "Genie/FS/stat.hh"
#include "Genie/FS/ResolvePathAt.hh"
#include "Genie/FS/ResolvePathname.hh"
#include "Genie/IO/RegularFile.hh"
#include "Genie/SystemCallRegistry.hh"


#ifndef AT_SYMLINK_NOFOLLOW
#define AT_SYMLINK_NOFOLLOW  0
#endif


namespace Genie
{
	
	static int faccessat( int dirfd, const char* path, int mode, int flags )
	{
		try
		{
			FSTreePtr file = ResolvePathAt( dirfd, path );
			
			if ( const bool following_links = !(flags & AT_SYMLINK_NOFOLLOW) )
			{
				ResolveLinks_InPlace( file );
			}
			
			if ( !exists( file ) )
			{
				return set_errno( EACCES );
			}
			
			// FIXME: check permissions
		}
		catch ( ... )
		{
			return set_errno_from_exception();
		}
		
		return 0;
	}
	
	
	static int fchmodat( int dirfd, const char* path, mode_t mode, int flags )
	{
		try
		{
			FSTreePtr file = ResolvePathAt( dirfd, path );
			
			if ( const bool following_links = !(flags & AT_SYMLINK_NOFOLLOW) )
			{
				ResolveLinks_InPlace( file );
			}
			
			chmod( file.get(), mode );
		}
		catch ( ... )
		{
			return set_errno_from_exception();
		}
		
		return 0;
	}
	
	static int fchmod( int fd, mode_t mode )
	{
		try
		{
			chmod( GetFileHandle( fd )->GetFile().get(), mode );
		}
		catch ( ... )
		{
			return set_errno_from_exception();
		}
		
		return 0;
	}
	
	
	static int fstatat( int dirfd, const char* path, struct stat* sb, int flags )
	{
		std::memset( (void*) sb, '\0', sizeof (struct stat) );
		
		sb->st_size = off_t( -1 );
		
		try
		{
			FSTreePtr file = ResolvePathAt( dirfd, path );
			
			if ( const bool following_links = !(flags & AT_SYMLINK_NOFOLLOW) )
			{
				ResolveLinks_InPlace( file );
			}
			
			stat( file.get(), *sb );
			
			if ( sb->st_size == off_t( -1 ) )
			{
				sb->st_size = geteof( file.get() );
			}
		}
		catch ( ... )
		{
			return set_errno_from_exception();
		}
		
		return 0;
	}
	
	
	static int fstat( int fd, struct stat* sb )
	{
		std::memset( (void*) sb, '\0', sizeof (struct stat) );
		
		sb->st_size = off_t( -1 );
		
		try
		{
			IOHandle& handle = *GetFileHandle( fd );
			
			stat( handle.GetFile().get(), *sb );
			
			if ( sb->st_size == off_t( -1 ) )
			{
				try
				{
					sb->st_size = IOHandle_Cast< RegularFileHandle >( handle ).GetEOF();
				}
				catch ( ... )
				{
					sb->st_size = 0;
				}
			}
		}
		catch ( ... )
		{
			return set_errno_from_exception();
		}
		
		return 0;
	}
	
	#pragma force_active on
	
	REGISTER_SYSTEM_CALL( faccessat );
	REGISTER_SYSTEM_CALL( fchmodat  );
	REGISTER_SYSTEM_CALL( fchmod    );
	REGISTER_SYSTEM_CALL( fstatat   );
	REGISTER_SYSTEM_CALL( fstat     );
	
	#pragma force_active reset
	
}

