/*
	Handle.cc
	---------
*/

#include "Genie/IO/Handle.hh"

// POSIX
#include <fcntl.h>

// more-libc
#include "more/string.h"

// Nitrogen
#include "Nitrogen/MacMemory.hh"

// poseven
#include "poseven/types/errno_t.hh"

// vfs
#include "vfs/node.hh"
#include "vfs/filehandle/methods/bstore_method_set.hh"
#include "vfs/filehandle/methods/filehandle_method_set.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	namespace p7 = poseven;
	
	
	template < class T >
	static inline T min( T a, T b )
	{
		return b < a ? b : a;
	}
	
	
	static
	void dispose_Mac_Handle( vfs::filehandle* that )
	{
		Mac_Handle_extra& extra = *(Mac_Handle_extra*) that->extra();
		
		::DisposeHandle( extra.handle );
	}
	
	
	static
	ssize_t Mac_Handle_pread( vfs::filehandle*  that,
	                          char*             buffer,
	                          size_t            n,
	                          off_t             offset );
	
	static
	off_t Mac_Handle_geteof( vfs::filehandle* that );
	
	static
	ssize_t Mac_Handle_pwrite( vfs::filehandle*  that,
	                           const char*       buffer,
	                           size_t            n,
	                           off_t             offset );
	
	static
	void Mac_Handle_seteof( vfs::filehandle* that, off_t length );
	
	static const vfs::bstore_method_set Mac_Handle_bstore_methods =
	{
		&Mac_Handle_pread,
		&Mac_Handle_geteof,
		&Mac_Handle_pwrite,
		&Mac_Handle_seteof,
	};
	
	const vfs::filehandle_method_set Mac_Handle_methods =
	{
		&Mac_Handle_bstore_methods,
	};
	
	
	vfs::filehandle* new_Handle_handle( const vfs::node&               file,
	                                    int                            flags,
	                                    nucleus::owned< Mac::Handle >  h )
	{
		using vfs::filehandle;
		
		filehandle* result = new filehandle( &file,
		                                     flags,
		                                     &Mac_Handle_methods,
		                                     sizeof (Mac_Handle_extra),
		                                     &dispose_Mac_Handle );
		
		Mac_Handle_extra& extra = *(Mac_Handle_extra*) result->extra();
		
		extra.handle = h.release();
		
		return result;
	}
	
	static
	ssize_t Mac_Handle_pread( vfs::filehandle*  that,
	                          char*             buffer,
	                          size_t            n_bytes,
	                          off_t             offset )
	{
		Mac_Handle_extra& extra = *(Mac_Handle_extra*) that->extra();
		
		const size_t size = ::GetHandleSize( extra.handle );
		
		if ( offset >= size )
		{
			return 0;
		}
		
		n_bytes = min< size_t >( n_bytes, size - offset );
		
		mempcpy( buffer, *extra.handle + offset, n_bytes );
		
		return n_bytes;
	}
	
	static
	ssize_t Mac_Handle_pwrite( vfs::filehandle*  that,
	                           const char*       buffer,
	                           size_t            n_bytes,
	                           off_t             offset )
	{
		Mac_Handle_extra& extra = *(Mac_Handle_extra*) that->extra();
		
		const bool writable = that->get_flags() + (1 - O_RDONLY) & 2;
		
		if ( !writable )
		{
			p7::throw_errno( EPERM );
		}
		
		if ( n_bytes == 0 )
		{
			return 0;
		}
		
		const size_t required_size = offset + n_bytes;
		
		const size_t existing_size = ::GetHandleSize( extra.handle );
		
		if ( required_size > existing_size )
		{
			::SetHandleSize( extra.handle, required_size );
			
			if ( MemError() != noErr )
			{
				if ( offset > existing_size )
				{
					throw;
				}
				
				n_bytes = min< size_t >( n_bytes, existing_size - offset );
			}
		}
		
		mempcpy( *extra.handle + offset, buffer, n_bytes );
		
		return n_bytes;
	}
	
	static
	off_t Mac_Handle_geteof( vfs::filehandle* that )
	{
		Mac_Handle_extra& extra = *(Mac_Handle_extra*) that->extra();
		
		return ::GetHandleSize( extra.handle );
	}
	
	static
	void Mac_Handle_seteof( vfs::filehandle* that, off_t length )
	{
		Mac_Handle_extra& extra = *(Mac_Handle_extra*) that->extra();
		
		N::SetHandleSize( extra.handle, length );
	}
	
}
