/*	==========
 *	symlink.cc
 *	==========
 */

// Standard C
#include <errno.h>

// POSIX
#include "sys/stat.h"
#include "unistd.h"

// poseven
#include "poseven/types/errno_t.hh"

// vfs
#include "vfs/node.hh"
#include "vfs/primitives/hardlink.hh"

// Genie
#include "Genie/current_process.hh"
#include "Genie/FS/file-tests.hh"
#include "Genie/FS/ResolvePathAt.hh"
#include "Genie/FS/ResolvePathname.hh"
#include "Genie/Kernel/make_alias.hh"
#include "Genie/SystemCallRegistry.hh"


#ifndef AT_SYMLINK_FOLLOW
#define AT_SYMLINK_FOLLOW  0
#endif

#ifndef AT_LINK_ALIAS
#define AT_LINK_ALIAS  0
#endif


namespace Genie
{
	
	namespace p7 = poseven;
	
	
	static int linkat( int olddirfd, const char* oldpath, int newdirfd, const char* newpath, int flags )
	{
		FSTreePtr oldFile = ResolvePathAt( olddirfd, oldpath );
		
		if ( const bool follow = flags & AT_SYMLINK_FOLLOW )
		{
			ResolveLinks_InPlace( oldFile );
		}
		
		try
		{
			FSTreePtr newFile = ResolvePathAt( newdirfd, newpath );
			
			// Do not resolve links.  If there's a symlink in this location, throw EEXIST.
			
			if ( exists( newFile ) )
			{
				return set_errno( EEXIST );
			}
			
			if ( flags & AT_LINK_ALIAS )
			{
				make_alias( oldFile, newFile );
			}
			else
			{
				hardlink( oldFile.get(), newFile.get() );
			}
		}
		catch ( ... )
		{
			return set_errno_from_exception();
		}
		
		return 0;
	}
	
	
	#pragma force_active on
	
	REGISTER_SYSTEM_CALL( linkat );
	
	#pragma force_active reset
	
}

