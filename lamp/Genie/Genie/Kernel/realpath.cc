/*	===========
 *	realpath.cc
 *	===========
 */

// Standard C
#include <errno.h>

// POSIX
#include <unistd.h>
#include <sys/stat.h>

// poseven
#include "poseven/types/errno_t.hh"

// MacLamp
#include "FSSpec_from_stat.h"

// GetPathname
#include "GetPathname.hh"

// Genie
#include "Genie/FS/ResolvePathAt.hh"
#include "Genie/FS/ResolvePathname.hh"
#include "Genie/SystemCallRegistry.hh"
#include "Genie/SystemCalls.hh"


#ifndef REALPATH_MAC
#define REALPATH_MAC  0
#endif


namespace Genie
{
	
	namespace p7 = poseven;
	
	
	static plus::string mac_pathname_from_file( const FSTreePtr& file )
	{
		struct ::stat stat_buffer;
		
		try
		{
			file->Stat( stat_buffer );
		}
		catch ( const p7::errno_t& err )
		{
			if ( err != ENOENT )
			{
				throw;
			}
		}
		
		FSSpec spec;
		
		p7::throw_posix_result( FSSpec_from_stat( stat_buffer, spec ) );
		
		return GetMacPathname( spec );
	}
	
	
	static ssize_t _realpathat( int dirfd, const char *path, char *buffer, size_t buffer_size, int flags )
	{
		SystemCallFrame frame( "_realpathat" );
		
		try
		{
			FSTreePtr file = ResolvePathAt( dirfd, path );
			
			ResolveLinks_InPlace( file );
			
			const bool is_mac = flags & REALPATH_MAC;
			
			plus::string resolved = is_mac ? mac_pathname_from_file( file )
			                               : file->Pathname();
			
			const bool too_big = resolved.size() > buffer_size;
			
			const size_t bytes_copied = std::min( buffer_size, resolved.size() );
			
			std::memcpy( buffer, resolved.data(), bytes_copied );
			
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
	
	REGISTER_SYSTEM_CALL( _realpathat );
	
	#pragma force_active reset
	
}

