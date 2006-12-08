/*	===========
 *	copyfile.cc
 *	===========
 */

// POSIX
#include "unistd.h"

// MoreFiles
#include "FileCopy.h"

// Nitrogen
#include "Nitrogen/Files.h"
#include "Nitrogen/OSStatus.h"

// Genie
#include "Genie/pathnames.hh"
#include "Genie/Process.hh"
#include "Genie/SystemCallRegistry.hh"
#include "Genie/SystemCalls.hh"
#include "Genie/Yield.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	
	static void FSpFileCopy( const FSSpec&         source,
	                         const FSSpec&         destDir,
	                         const unsigned char*  copyName       = NULL,
	                         void*                 copyBufferPtr  = NULL,
	                         long                  copyBufferSize = 0,
	                         bool                  preflight      = true )
	{
		(void) N::FileManagerErrorsRegistrationDependency();
		
		N::ThrowOSStatus( ::FSpFileCopy( &source,
		                                 &destDir,
		                                 copyName,
		                                 copyBufferPtr,
		                                 copyBufferSize,
		                                 preflight ) );
	}
	
	static int copyfile( const char* src, const char* dest )
	{
		try
		{
			const N::FSDirSpec cwd = CurrentProcess().CurrentDirectory();
			
			const FSSpec srcFile  = ResolveUnixPathname( src,  cwd );
			const FSSpec destFile = ResolveUnixPathname( dest, cwd );
			
			FSpFileCopy( srcFile, destFile );
		}
		catch ( ... )
		{
			return GetErrnoFromExceptionInSystemCall();
		}
		
		return 0;
	}
	
	REGISTER_SYSTEM_CALL( copyfile );
	
}

