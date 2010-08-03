/*	================
 *	ResolvePathAt.cc
 *	================
 */

#include "Genie/FS/ResolvePathAt.hh"

// POSIX
#include "fcntl.h"

// POSeven
#include "poseven/types/errno_t.hh"

// Genie
#include "Genie/FileDescriptors.hh"
#include "Genie/FS/ResolvePathname.hh"
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
		
		const bool absolute = path[0] == '/';
		
		return ResolvePathname( path,   absolute          ? null_FSTreePtr
		                              : dirfd == AT_FDCWD ? CurrentProcess().GetCWD()
			                          :                     GetDirFile( dirfd ) );
	}
	
}

