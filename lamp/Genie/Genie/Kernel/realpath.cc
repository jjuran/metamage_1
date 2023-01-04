/*	===========
 *	realpath.cc
 *	===========
 */

// Standard C
#include <errno.h>

// POSIX
#include <unistd.h>
#include <sys/stat.h>

// more-libc
#include "more/string.h"

// plus
#include "plus/mac_utf8.hh"

// GetPathname
#include "GetMacPathname.hh"

// vfs
#include "vfs/node.hh"
#include "vfs/functions/pathname.hh"
#include "vfs/functions/resolve_links_in_place.hh"

// MacVFS
#include "MacVFS/util/FSSpec_from_node.hh"

// relix-kernel
#include "relix/api/errno.hh"
#include "relix/api/root.hh"
#include "relix/fs/resolve_path_at.hh"
#include "relix/syscall/registry.hh"

// Genie


#ifndef REALPATH_OUTPUT_HFS
#define REALPATH_OUTPUT_HFS (-1)
#endif

#ifndef REALPATH_OUTPUT_HFS_UTF8
#define REALPATH_OUTPUT_HFS_UTF8 (-1)
#endif


namespace Genie
{
	
	template < class T >
	static inline T min( T a, T b )
	{
		return b < a ? b : a;
	}
	
	
	static plus::string mac_pathname_from_file( const vfs::node& file )
	{
		return GetMacPathname( vfs::FSSpec_from_node( file ) );
	}
	
	
	static ssize_t _realpathat( int dirfd, const char *path, char *buffer, size_t buffer_size, int flags )
	{
		try
		{
			vfs::node_ptr file = relix::resolve_path_at( dirfd, path );
			
			vfs::resolve_links_in_place( *relix::root(), file );
			
			const bool is_mac = flags & REALPATH_OUTPUT_HFS;
			
			plus::string resolved = is_mac ? mac_pathname_from_file( *file )
			                               : pathname( *file );
			
			if ( (flags & REALPATH_OUTPUT_HFS_UTF8) == REALPATH_OUTPUT_HFS_UTF8 )
			{
				resolved = plus::utf8_from_mac( resolved );
			}
			
			const bool too_big = resolved.size() > buffer_size;
			
			const size_t bytes_copied = min( buffer_size, resolved.size() );
			
			mempcpy( buffer, resolved.data(), bytes_copied );
			
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
			return relix::set_errno_from_exception();
		}
	}
	
	#pragma force_active on
	
	REGISTER_SYSTEM_CALL( _realpathat );
	
	#pragma force_active reset
	
}
