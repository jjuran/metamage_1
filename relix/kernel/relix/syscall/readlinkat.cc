/*
	readlinkat.cc
	-------------
*/

#include "relix/syscall/readlinkat.hh"

// Standard C
#include <errno.h>

// more-libc
#include "more/string.h"

// vfs
#include "vfs/node.hh"
#include "vfs/functions/file-tests.hh"
#include "vfs/primitives/readlink.hh"

// relix
#include "relix/api/errno.hh"
#include "relix/fs/resolve_path_at.hh"


namespace relix
{
	
	static inline size_t min( size_t a, size_t b )
	{
		return a > b ? b : a;
	}
	
	// Our variant of readlink() provides complete information regarding the size
	// of the result data, even in the case of too small a buffer.
	
	// Whereas readlink() returns the number of bytes copied (which leaves you
	// uninformed of the actual data size, unless the buffer length exceeds it,
	// *even if the buffer is (just) large enough*), and readlinkat() tells you
	// nothing at all (other than that it succeeded) unless you preload the
	// buffer entirely with null bytes, _readlinkat() always returns the data
	// size in some form if it returns data at all.
	
	// Like readlink() and readlinkat(), _readlinkat() will copy as much data as
	// will fit in the buffer.  If all the data fit in the buffer, the data size
	// is returned.  But if the data exceed the buffer length, then errno is set
	// to ERANGE (as for getcwd()) and the bitwise inverse of the data size is
	// returned, which will invariably be a negative number.  The additive
	// inverse of the return value in this case equals the data size plus one.
	
	ssize_t _readlinkat( int dirfd, const char *path, char *buffer, size_t buffer_size, int flags )
	{
		try
		{
			vfs::node_ptr link = resolve_path_at( dirfd, path );
			
			// Do not resolve links -- we want the target even if it's another symlink
			
			if ( !is_symlink( *link ) )
			{
				return set_errno( exists( *link ) ? EINVAL : ENOENT );
			}
			
			plus::string linkPath = readlink( *link );
			
			const bool too_big = linkPath.size() > buffer_size;
			
			const size_t bytes_copied = min( buffer_size, linkPath.size() );
			
			mempcpy( buffer, linkPath.data(), bytes_copied );
			
			if ( too_big )
			{
				errno = ERANGE;
				
				// Return the bitwise inverse of the data size.
				return ~linkPath.size();
			}
			
			return bytes_copied;
		}
		catch ( ... )
		{
			return set_errno_from_exception();
		}
	}
	
}
