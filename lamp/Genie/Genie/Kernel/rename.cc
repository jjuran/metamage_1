/*	=========
 *	rename.cc
 *	=========
 */

// Genie
#include "Genie/FS/ResolvePathAt.hh"
#include "Genie/SystemCallRegistry.hh"
#include "Genie/SystemCalls.hh"


namespace Genie
{
	
	static int renameat( int olddirfd, const char* oldpath, int newdirfd, const char* newpath )
	{
		SystemCallFrame frame( "renameat" );
		
		try
		{
			FSTreePtr srcFile  = ResolvePathAt( olddirfd, oldpath );
			FSTreePtr destFile = ResolvePathAt( newdirfd, newpath );
			
			// Do not resolve links
			
			srcFile->Rename( destFile );
		}
		catch ( ... )
		{
			return frame.SetErrnoFromException();
		}
		
		return 0;
	}
	
	#pragma force_active on
	
	REGISTER_SYSTEM_CALL( renameat );
	
	#pragma force_active reset
	
}

