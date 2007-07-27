/*	========
 *	mkdir.cc
 *	========
 */

// POSIX
#include "sys/stat.h"

// Genie
#include "Genie/FileSystem/ResolvePathname.hh"
#include "Genie/Process.hh"
#include "Genie/SystemCallRegistry.hh"
#include "Genie/SystemCalls.hh"
#include "Genie/Yield.hh"


namespace Genie
{
	
	DECLARE_MODULE_INIT( Kernel_mkdir )
	DEFINE_MODULE_INIT(  Kernel_mkdir )
	
	static int mkdir( const char* pathname, mode_t mode )
	{
		SystemCallFrame frame( "mkdir" );
		
		try
		{
			FSTreePtr current = CurrentProcess().GetCWD();
			
			FSTreePtr location = ResolvePathname( pathname, current );
			
			location->CreateDirectory( mode );
		}
		catch ( ... )
		{
			return frame.SetErrnoFromException();
		}
		
		return 0;
	}
	
	REGISTER_SYSTEM_CALL( mkdir );
	
}

