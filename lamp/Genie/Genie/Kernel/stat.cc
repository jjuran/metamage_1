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
#include "vfs/node.hh"
#include "vfs/filehandle/primitives/geteof.hh"
#include "vfs/functions/resolve_links_in_place.hh"
#include "vfs/primitives/chmod.hh"
#include "vfs/primitives/geteof.hh"
#include "vfs/primitives/stat.hh"

// Genie
#include "Genie/current_process.hh"
#include "Genie/FileDescriptors.hh"
#include "Genie/FS/file-tests.hh"
#include "Genie/FS/ResolvePathAt.hh"
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
				vfs::resolve_links_in_place( file );
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
				vfs::resolve_links_in_place( file );
			}
			
			chmod( *file, mode );
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
			chmod( *get_filehandle( fd ).GetFile(), mode );
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
				vfs::resolve_links_in_place( file );
			}
			
			stat( file.get(), *sb );
			
			if ( sb->st_size == off_t( -1 ) )
			{
				sb->st_size = geteof( *file );
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
			IOHandle& handle = get_filehandle( fd );
			
			stat( handle.GetFile().get(), *sb );
			
			if ( sb->st_size == off_t( -1 ) )
			{
				try
				{
					sb->st_size = geteof( handle );
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

