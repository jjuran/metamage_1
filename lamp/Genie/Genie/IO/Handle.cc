/*
	Handle.cc
	---------
*/

#include "Genie/IO/Handle.hh"

// POSIX
#include <fcntl.h>

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
	
	
	static ssize_t Mac_Handle_pread( vfs::filehandle* file, char* buffer, size_t n, off_t offset )
	{
		return static_cast< Handle_IOHandle& >( *file ).Positioned_Read( buffer, n, offset );
	}
	
	static off_t Mac_Handle_geteof( vfs::filehandle* file )
	{
		return static_cast< Handle_IOHandle& >( *file ).GetEOF();
	}
	
	static ssize_t Mac_Handle_pwrite( vfs::filehandle* file, const char* buffer, size_t n, off_t offset )
	{
		return static_cast< Handle_IOHandle& >( *file ).Positioned_Write( buffer, n, offset );
	}
	
	static void Mac_Handle_seteof( vfs::filehandle* file, off_t length )
	{
		static_cast< Handle_IOHandle& >( *file ).SetEOF( length );
	}
	
	static const vfs::bstore_method_set Mac_Handle_bstore_methods =
	{
		&Mac_Handle_pread,
		&Mac_Handle_geteof,
		&Mac_Handle_pwrite,
		&Mac_Handle_seteof,
	};
	
	static const vfs::filehandle_method_set Mac_Handle_methods =
	{
		&Mac_Handle_bstore_methods,
	};
	
	
	Handle_IOHandle::Handle_IOHandle( const vfs::node&                       file,
	                                  int                                    flags,
	                                  const nucleus::shared< Mac::Handle >&  h )
	:
		vfs::filehandle( &file, flags, &Mac_Handle_methods ),
		itsHandle( h )
	{
	}
	
	Handle_IOHandle::~Handle_IOHandle()
	{
	}
	
	ssize_t Handle_IOHandle::Positioned_Read( char* buffer, size_t n_bytes, off_t offset )
	{
		const size_t size = GetEOF();
		
		if ( offset >= size )
		{
			return 0;
		}
		
		n_bytes = min< size_t >( n_bytes, size - offset );
		
		memcpy( buffer, *itsHandle.get().Get() + offset, n_bytes );
		
		return n_bytes;
	}
	
	ssize_t Handle_IOHandle::Positioned_Write( const char* buffer, size_t n_bytes, off_t offset )
	{
		const bool writable = get_flags() + (1 - O_RDONLY) & 2;
		
		if ( !writable )
		{
			p7::throw_errno( EPERM );
		}
		
		if ( n_bytes == 0 )
		{
			return 0;
		}
		
		const size_t required_size = offset + n_bytes;
		
		const size_t existing_size = GetEOF();
		
		if ( required_size > existing_size )
		{
			try
			{
				N::SetHandleSize( itsHandle, required_size );
			}
			catch ( ... )
			{
				if ( offset > existing_size )
				{
					throw;
				}
				
				n_bytes = min< size_t >( n_bytes, existing_size - offset );
			}
		}
		
		memcpy( *itsHandle.get().Get() + offset, buffer, n_bytes );
		
		return n_bytes;
	}
	
	off_t Handle_IOHandle::GetEOF()
	{
		return N::GetHandleSize( itsHandle );
	}
	
	void Handle_IOHandle::SetEOF( off_t length )
	{
		N::SetHandleSize( itsHandle, length );
	}
	
}

