/*	===========
 *	realpath.cc
 *	===========
 */

// Standard C
#include <errno.h>

// POSIX
#include <unistd.h>

// Genie
#include "Genie/FS/ResolvePathname.hh"
#include "Genie/Process.hh"
#include "Genie/SystemCallRegistry.hh"
#include "Genie/SystemCalls.hh"


namespace Genie
{
	
	static ssize_t realpath_k( const char* pathname, char* resolved_path, size_t buffer_size )
	{
		SystemCallFrame frame( "realpath_k" );
		
		try
		{
			FSTreePtr cwd = frame.Caller().GetCWD();
			
			FSTreePtr file = ResolvePathname( pathname, cwd );
			
			ResolveLinks_InPlace( file );
			
			std::string resolved = file->Pathname();
			
			const bool too_big = resolved.size() > buffer_size;
			
			const size_t bytes_copied = std::min( buffer_size, resolved.size() );
			
			std::memcpy( resolved_path, resolved.data(), bytes_copied );
			
			if ( too_big )
			{
				errno = ERANGE;
				
				// Return the bitwise inverse of the data size.
				return ~resolved.size();
			}
			
			return bytes_copied;
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

