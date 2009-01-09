/*	===========
 *	copyfile.cc
 *	===========
 */

// POSIX
#include <unistd.h>

// Genie
#include "Genie/FileSystem/ResolvePathname.hh"
#include "Genie/Process.hh"
#include "Genie/SystemCallRegistry.hh"
#include "Genie/SystemCalls.hh"


namespace Genie
{
	
	static int copyfile( const char* src, const char* dest )
	{
		SystemCallFrame frame( "copyfile" );
		
		try
		{
			FSTreePtr cwd = frame.Caller().GetCWD();
			
			FSTreePtr srcFile  = ResolvePathname( src,  cwd );
			FSTreePtr destFile = ResolvePathname( dest, cwd );
			
			// Do not resolve links
			
			srcFile->CopyFile( destFile );
		}
		catch ( ... )
		{
			return frame.SetErrnoFromException();
		}
		
		return 0;
	}
	
	#pragma force_active on
	
	REGISTER_SYSTEM_CALL( copyfile );
	
	#pragma force_active reset
	
}

