/*	===========
 *	copyfile.cc
 *	===========
 */

// POSIX
#include <unistd.h>

// MoreFiles
#include "FileCopy.h"

// Nitrogen
#include "Nitrogen/Files.h"
#include "Nitrogen/OSStatus.h"

// Genie
#include "Genie/FileSystem/ResolvePathname.hh"
#include "Genie/Process.hh"
#include "Genie/SystemCallRegistry.hh"
#include "Genie/SystemCalls.hh"
#include "Genie/Yield.hh"


namespace Genie
{
	
	DECLARE_MODULE_INIT( Kernel_copyfile )
	DEFINE_MODULE_INIT(  Kernel_copyfile )
	
	namespace N = Nitrogen;
	
	using namespace io::path_descent_operators;
	
	
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
		SystemCallFrame frame( "copyfile" );
		
		try
		{
			FSTreePtr cwd = CurrentProcess().GetCWD();
			
			const FSSpec srcFile  = ResolvePathname( src,  cwd )->GetFSSpec();
			const FSSpec destFile = ResolvePathname( dest, cwd )->GetFSSpec();
			
			N::FSDirSpec destDir = io::get_preceding_directory( destFile );
			
			bool renaming = !std::equal( srcFile.name,
			                             srcFile.name + 1 + srcFile.name[0],
			                             destFile.name );
			
			ConstStr255Param name = renaming ? destFile.name : NULL;
			
			// FIXME:  This logic should be worked into the file copy routine
			// Maybe use ExchangeFiles() for safety?
			
			if ( io::file_exists( destFile ) )
			{
				io::delete_file( destFile );
			}
			
			FSpFileCopy( srcFile, destDir / "", name );
		}
		catch ( ... )
		{
			return frame.GetErrnoFromException();
		}
		
		return 0;
	}
	
	REGISTER_SYSTEM_CALL( copyfile );
	
}

