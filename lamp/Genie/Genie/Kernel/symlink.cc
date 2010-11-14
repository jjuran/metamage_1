/*	==========
 *	symlink.cc
 *	==========
 */

// Standard C
#include <errno.h>

// Genie
#include "Genie/current_process.hh"
#include "Genie/FS/FSTree.hh"
#include "Genie/FS/ResolvePathAt.hh"
#include "Genie/SystemCallRegistry.hh"
#include "Genie/SystemCalls.hh"


namespace Genie
{
	
	static int symlinkat( const char* target_path, int newdirfd, const char* newpath )
	{
		SystemCallFrame frame( "symlinkat" );
		
		try
		{
			FSTreePtr link = ResolvePathAt( newdirfd, newpath );
			
			// Do not resolve links.  If there's a symlink in this location, throw EEXIST.
			
			if ( link->Exists() )
			{
				return set_errno( EEXIST );
			}
			
			link->SymLink( target_path );
		}
		catch ( ... )
		{
			return set_errno_from_exception();
		}
		
		return 0;
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
	
	static ssize_t _readlinkat( int dirfd, const char *path, char *buffer, size_t buffer_size, int flags )
	{
		SystemCallFrame frame( "_readlinkat" );
		
		try
		{
			FSTreePtr link = ResolvePathAt( dirfd, path );
			
			// Do not resolve links -- we want the target even if it's another symlink
			
			if ( !link->IsLink() )
			{
				return set_errno( link->Exists() ? EINVAL : ENOENT );
			}
			
			plus::string linkPath = link->ReadLink();
			
			const bool too_big = linkPath.size() > buffer_size;
			
			const size_t bytes_copied = std::min( buffer_size, linkPath.size() );
			
			std::memcpy( buffer, linkPath.data(), bytes_copied );
			
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
	
	#pragma force_active on
	
	REGISTER_SYSTEM_CALL( symlinkat   );
	REGISTER_SYSTEM_CALL( _readlinkat );
	
	#pragma force_active reset
	
}

