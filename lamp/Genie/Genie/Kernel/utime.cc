/*	========
 *	utime.cc
 *	========
 */

// Standard C++
#include <algorithm>

// POSIX
#include "sys/utsname.h"

// Nucleus
#include "Nucleus/NAssert.h"

// Genie
#include "Genie/Process.hh"
#include "Genie/FileSystem/ResolvePathname.hh"
#include "Genie/SystemCallRegistry.hh"
#include "Genie/SystemCalls.hh"


namespace Genie
{
	
	static int utime( const char* path, const struct utimbuf *time_buffer )
	{
		SystemCallFrame frame( "utime" );
		
		try
		{
			FSTreePtr current = frame.Caller().GetCWD();
			
			FSTreePtr file = ResolvePathname( path, current );
			
			ResolveLinks_InPlace( file );
			
			file->SetUTime( time_buffer );
		}
		catch ( ... )
		{
			return frame.SetErrnoFromException();
		}
		
		return 0;
	}
	
	#pragma force_active on
	
	REGISTER_SYSTEM_CALL( utime );
	
	#pragma force_active reset
	
}

