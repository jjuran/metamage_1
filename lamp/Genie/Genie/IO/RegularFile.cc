/*	==============
 *	RegularFile.cc
 *	==============
 */

#include "Genie/IO/RegularFile.hh"

// vfs
#include "vfs/node.hh"
#include "vfs/filehandle/primitives/pread.hh"
#include "vfs/mmap/functions/map_uninitialized.hh"
#include "vfs/mmap/types/file_memory_mapping.hh"


namespace Genie
{
	
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
		vfs::memory_mapping_ptr result = vfs::map_uninitialized( length, prot, flags );
		
		vfs::memory_mapping* mapping = result.get();
		
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

