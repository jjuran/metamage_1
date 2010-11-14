/*	=========
 *	rename.cc
 *	=========
 */

// Genie
#include "Genie/current_process.hh"
#include "Genie/FS/ResolvePathAt.hh"
#include "Genie/SystemCallRegistry.hh"


namespace Genie
{
	
	static int renameat( int olddirfd, const char* oldpath, int newdirfd, const char* newpath )
	{
		try
		{
			FSTreePtr srcFile  = ResolvePathAt( olddirfd, oldpath );
			FSTreePtr destFile = ResolvePathAt( newdirfd, newpath );
			
			// Do not resolve links
			
			srcFile->Rename( destFile );
		}
		catch ( ... )
		{
			return set_errno_from_exception();
		}
		
		return 0;
	}
	
	#pragma force_active on
	
	REGISTER_SYSTEM_CALL( renameat );
	
	#pragma force_active reset
	
}

