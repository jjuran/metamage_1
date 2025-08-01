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

// gear
#include "gear/parse_decimal.hh"

// vfs
#include "vfs/node.hh"
#include "vfs/functions/pathname.hh"
#include "vfs/functions/resolve_links_in_place.hh"

// relix-kernel
#include "relix/api/errno.hh"
#include "relix/api/root.hh"
#include "relix/fs/resolve_path_at.hh"
#include "relix/syscall/registry.hh"

// Genie
#include "Genie/FS/FSSpec.hh"


#define STRLEN(s)  (sizeof "" s - 1)

#define STR_LEN(s)  "" s, (sizeof s - 1)


namespace Genie
{

template < class T >
static inline
T min( T a, T b )
{
	return b < a ? b : a;
}

static inline
bool begins_with( const char* s, const char* prefix, size_t length )
{
	while ( length-- )
	{
		if ( *s++ != *prefix++ )
		{
			return false;
		}
	}
	
	return true;
}


static
ssize_t _realpathat( int dirfd, const char* path, char* p, size_t n, int flags )
{
	short vRefNum = 0;
	long  dirID   = 0;
	
	if ( begins_with( path, STR_LEN( "/.hfs/" ) ) )
	{
		if ( flags )
		{
			return relix::set_errno( EINVAL );
		}
		
		const char* p = path + STRLEN( "/.hfs/" );
		
		unsigned v = gear::parse_unsigned_decimal( &p );
		
		if ( v == 0  ||  v > 0x7fff )
		{
			return relix::set_errno( ENOENT );
		}
		
		if ( *p++ != '/' )
		{
			return relix::set_errno( ENXIO );
		}
		
		unsigned d = gear::parse_unsigned_decimal( &p );
		
		if ( d == 0  ||  d > 0x7fffffff )
		{
			return relix::set_errno( ENOENT );
		}
		
		if ( *p == '/' )
		{
			++p;
		}
		
		if ( *p != '\0' )
		{
			return relix::set_errno( ENOENT );
		}
		
		vRefNum = -v;
		dirID   =  d;
	}
	
	try
	{
		vfs::node_ptr file = dirID ? node_from_dirID( vRefNum, dirID )
		                           : relix::resolve_path_at( dirfd, path );
		
		vfs::resolve_links_in_place( *relix::root(), file );
		
		plus::string resolved = pathname( *file );
		
		const size_t resolved_size = resolved.size();
		
		const bool too_big = resolved_size > n;
		
		const size_t bytes_copied = min( n, resolved_size );
		
		mempcpy( p, resolved.data(), bytes_copied );
		
		if ( too_big )
		{
			errno = ERANGE;
			
			// Return the bitwise inverse of the data size.
			return ~resolved_size;
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
