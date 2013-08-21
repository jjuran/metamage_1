/*	================
 *	ResolvePathAt.cc
 *	================
 */

#include "Genie/FS/ResolvePathAt.hh"

// POSIX
#include "fcntl.h"

// POSeven
#include "poseven/types/errno_t.hh"

// vfs
#include "vfs/node.hh"
#include "vfs/functions/resolve_pathname.hh"

// Genie
#include "Genie/FileDescriptors.hh"
#include "Genie/IO/Directory.hh"
#include "Genie/Process.hh"


#ifndef AT_FDCWD
#define AT_FDCWD  (-100)
#endif


namespace Genie
{
	
	namespace p7 = poseven;
	
	
	static FSTreePtr GetDirFile( int fd )
	{
		return GetFileHandleWithCast< DirHandle >( fd ).GetFile();
	}
	
	FSTreePtr ResolvePathAt( int dirfd, const plus::string& path )
	{
		if ( path.empty() )
		{
			p7::throw_errno( ENOENT );
		}
		
		if ( path[0] == '/' )
		{
			return vfs::resolve_absolute_path( path );
		}
		
		vfs::node_ptr at_dir = dirfd == AT_FDCWD ? CurrentProcess().GetCWD()
		                                         : GetDirFile( dirfd );
		
		return resolve_pathname( path, *at_dir );
	}
	
}

