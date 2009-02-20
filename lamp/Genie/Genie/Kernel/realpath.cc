/*	===========
 *	realpath.cc
 *	===========
 */

// Standard C++
#include <algorithm>

// POSIX
#include <unistd.h>

// Genie
#include "Genie/FileSystem/ResolvePathname.hh"
#include "Genie/Process.hh"
#include "Genie/SystemCallRegistry.hh"
#include "Genie/SystemCalls.hh"


namespace Genie
{
	
	static ssize_t realpath_k( const char* pathname, char* resolved_path, size_t size )
	{
		SystemCallFrame frame( "realpath_k" );
		
		try
		{
			FSTreePtr cwd = frame.Caller().GetCWD();
			
			FSTreePtr file = ResolvePathname( pathname, cwd );
			
			ResolveLinks_InPlace( file );
			
			std::string resolved = file->Pathname();
			
			size = std::min( size, resolved.size() );
			
			std::copy( resolved.begin(), resolved.begin() + size, resolved_path );
			
			return resolved.size();
		}
		catch ( ... )
		{
			return frame.SetErrnoFromException();
		}
	}
	
	#pragma force_active on
	
	REGISTER_SYSTEM_CALL( realpath_k );
	
	#pragma force_active reset
	
}

