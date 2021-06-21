/*
	buffer_file.cc
	--------------
*/

#include "vfs/filehandle/types/buffer_file.hh"

// more-libc
#include "more/string.h"

// poseven
#include "poseven/types/errno_t.hh"

// vfs
#include "vfs/filehandle.hh"
#include "vfs/memory_mapping.hh"
#include "vfs/node.hh"
#include "vfs/filehandle/methods/bstore_method_set.hh"
#include "vfs/filehandle/methods/filehandle_method_set.hh"
#include "vfs/filehandle/methods/general_method_set.hh"


namespace vfs
{
	
	namespace p7 = poseven;
	
	
	struct buffer_extra
	{
		char*   base;  // base address
		size_t  size;
	};
	
	template < class T >
	static inline T min( T a, T b )
	{
		return b < a ? b : a;
	}
	
	
	static ssize_t buffer_pread( filehandle* file, char* buffer, size_t n, off_t offset )
	{
		buffer_extra& extra = *(buffer_extra*) file->extra();
		
		if ( offset >= extra.size )
		{
			return 0;
		}
		
		n = min< size_t >( n, extra.size - offset );
		
		mempcpy( buffer, extra.base + offset, n );
		
		return n;
	}
	
	static off_t buffer_geteof( filehandle* file )
	{
		buffer_extra& extra = *(buffer_extra*) file->extra();
		
		return extra.size;
	}
	
	static ssize_t buffer_pwrite( filehandle* file, const char* buffer, size_t n, off_t offset )
	{
		buffer_extra& extra = *(buffer_extra*) file->extra();
		
		if ( n == 0 )
		{
			return 0;
		}
		
		if ( offset >= extra.size )
		{
			p7::throw_errno( ENOSPC );
		}
		
		n = min< size_t >( n, extra.size - offset );
		
		mempcpy( extra.base + offset, buffer, n );
		
		return n;
	}
	
	static memory_mapping_ptr buffer_mmap( filehandle* that, size_t length, int prot, int flags, off_t offset )
	{
		buffer_extra& extra = *(buffer_extra*) that->extra();
		
		if ( offset + length > extra.size )
		{
			p7::throw_errno( ENXIO );
		}
		
		return new memory_mapping( extra.base + offset, length, flags );
	}
	
	static const bstore_method_set buffer_bstore_methods =
	{
		&buffer_pread,
		&buffer_geteof,
		&buffer_pwrite,
	};
	
	static const general_method_set buffer_general_methods =
	{
		&buffer_mmap,
	};
	
	static const filehandle_method_set buffer_methods =
	{
		&buffer_bstore_methods,
		NULL,
		NULL,
		&buffer_general_methods,
	};
	
	
	filehandle_ptr open_buffer_file( const node&  file,
	                                 int          flags,
	                                 char*        addr,
	                                 std::size_t  size )
	{
		filehandle* result = new filehandle( &file,
		                                     flags,
		                                     &buffer_methods,
		                                     sizeof (buffer_extra) );
		
		buffer_extra& extra = *(buffer_extra*) result->extra();
		
		extra.base = addr;
		extra.size = size;
		
		return result;
	}
	
}
