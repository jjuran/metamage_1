/*	==============
 *	RegularFile.cc
 *	==============
 */

#include "Genie/IO/RegularFile.hh"

// Standard C
#include <stdlib.h>

// POSIX
#include <fcntl.h>

// Nitrogen
#include "Nitrogen/MacMemory.hh"

// poseven
#include "poseven/types/errno_t.hh"

// vfs
#include "vfs/node.hh"
#include "vfs/filehandle/primitives/pread.hh"
#include "vfs/mmap/types/file_memory_mapping.hh"

// MacVFS
#include "MacVFS/mmap/Handle_memory_mapping.hh"


namespace Genie
{
	
	namespace n = nucleus;
	namespace N = Nitrogen;
	namespace p7 = poseven;
	
	
	class malloc_memory_mapping : public vfs::memory_mapping
	{
		private:
			// non-copyable
			malloc_memory_mapping           ( const malloc_memory_mapping& );
			malloc_memory_mapping& operator=( const malloc_memory_mapping& );
		
		public:
			malloc_memory_mapping( void* addr, size_t size, int flags )
			:
				memory_mapping( addr, size, flags )
			{
			}
			
			~malloc_memory_mapping()
			{
				free( get_address() );
			}
	};
	
	
	RegularFileHandle::RegularFileHandle( int                                flags,
	                                      const vfs::filehandle_method_set*  methods )
	:
		vfs::filehandle( flags, methods )
	{
	}
	
	RegularFileHandle::RegularFileHandle( const vfs::node_ptr&               file,
	                                      int                                flags,
	                                      const vfs::filehandle_method_set*  methods )
	:
		vfs::filehandle( file.get(), flags, methods )
	{
	}
	
	RegularFileHandle::~RegularFileHandle()
	{
	}
	
	vfs::memory_mapping_ptr
	//
	RegularFileHandle::Map( size_t length, int prot, int flags, off_t offset )
	{
		vfs::memory_mapping* mapping = NULL;
		
		if ( const bool small = length < 64 * 1024 )
		{
			if ( void* addr = malloc( length ) )
			{
				mapping = new malloc_memory_mapping( addr, length, flags );
			}
		}
		
		if ( mapping == NULL )
		{
			// Either malloc() failed or we didn't try.
			
			n::owned< N::Handle > h = N::TempNewHandle( length );
			
			mapping = new vfs::Handle_memory_mapping( h, length, flags );
		}
		
		vfs::memory_mapping_ptr result( mapping );
		
		result = vfs::memory_mapping_ptr( new vfs::file_memory_mapping( mapping, this, offset ) );
		
		void* addr = mapping->get_address();
		
		ssize_t count = pread( *this, (char*) addr, length, offset );
		
		if ( count < length )
		{
			memset( (char*) addr + count, '\0', length - count );
		}
		
		return result;
	}
	
}

