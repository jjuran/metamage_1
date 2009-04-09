/*	==========
 *	symlink.cc
 *	==========
 */

// Standard C
#include <errno.h>

// POSIX
#include "sys/stat.h"
#include "unistd.h"

// Nitrogen
#include "Nitrogen/MacErrors.h"

// POSeven
#include "POSeven/Errno.hh"

// Genie
#include "Genie/FS/FSTree.hh"
#include "Genie/FS/ResolvePathAt.hh"
#include "Genie/SystemCallRegistry.hh"
#include "Genie/SystemCalls.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	namespace p7 = poseven;
	
	
	static int symlinkat( const char* target_path, int newdirfd, const char* newpath )
	{
		SystemCallFrame frame( "symlinkat" );
		
		try
		{
			FSTreePtr link = ResolvePathAt( newdirfd, newpath );
			
			// Do not resolve links.  If there's a symlink in this location, throw EEXIST.
			
			struct ::stat location_status;
			
			try
			{
				// Stat the location.  Throws ENOENT if nonexistent.
				link->Stat( location_status );
				
				// The new location also exists; bail.
				return frame.SetErrno( EEXIST );
			}
			catch ( const p7::errno_t& errnum )
			{
				if ( errnum != ENOENT )  throw;
			}
			catch ( const N::FNFErr& err )
			{
			#ifdef __MWERKS__
				
				if ( err.Get() != fnfErr )
				{
					throw;
				}
				
			#endif
			}
			
			// If we got here, link is a valid location.
			
			link->SymLink( target_path );
		}
		catch ( ... )
		{
			return frame.SetErrnoFromException();
		}
		
		return 0;
	}
	
	
	static ssize_t readlinkat_k( int dirfd, const char *path, char *buffer, size_t buffer_size )
	{
		SystemCallFrame frame( "readlinkat_k" );
		
		try
		{
			FSTreePtr link = ResolvePathAt( dirfd, path );
			
			// Do not resolve links -- we want the target even if it's another symlink
			
			std::string linkPath = link->ReadLink();
			
			buffer_size = std::min( buffer_size, linkPath.size() );
			
			std::copy( linkPath.begin(),
			           linkPath.begin() + buffer_size,
			           buffer );
			
			return linkPath.size();
		}
		catch ( ... )
		{
			return frame.SetErrnoFromException();
		}
	}
	
	#pragma force_active on
	
	REGISTER_SYSTEM_CALL( symlinkat    );
	REGISTER_SYSTEM_CALL( readlinkat_k );
	
	#pragma force_active reset
	
}

