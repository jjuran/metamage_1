/*	==============
 *	RegularFile.cc
 *	==============
 */

#include "Genie/IO/RegularFile.hh"

// Standard C
#include <stdlib.h>

// POSIX
#include <fcntl.h>
#include <sys/mman.h>

// Nitrogen
#include "Nitrogen/MacMemory.hh"

// poseven
#include "poseven/types/errno_t.hh"

// vfs
#include "vfs/node.hh"
#include "vfs/filehandle/primitives/geteof.hh"
#include "vfs/filehandle/primitives/pread.hh"
#include "vfs/filehandle/primitives/pwrite.hh"
#include "vfs/mmap/types/file_memory_mapping.hh"
#include "vfs/primitives/geteof.hh"
#include "vfs/primitives/seteof.hh"

// MacVFS
#include "MacVFS/mmap/Handle_memory_mapping.hh"


namespace Genie
{
	
	namespace n = nucleus;
	namespace N = Nitrogen;
	namespace p7 = poseven;
	
	
	class file_memory_mapping : public vfs::file_memory_mapping
	{
		private:
			off_t  its_offset;
		
		public:
			file_memory_mapping( memory_mapping*     memory,
			                     RegularFileHandle*  file,
			                     off_t               offset )
			:
				vfs::file_memory_mapping( memory, file ),
				its_offset( offset )
			{
			}
			
			~file_memory_mapping();
			
			off_t get_offset() const  { return its_offset; }
			
			void msync( void* addr, size_t len, int flags ) const;
	};
	
	file_memory_mapping::~file_memory_mapping()
	{
		msync( get_address(), get_size(), MS_SYNC );
	}
	
	void file_memory_mapping::msync( void* addr, size_t len, int flags ) const
	{
		const int mmap_flags = get_flags();
		
		if ( (mmap_flags & (MAP_SHARED|MAP_PRIVATE)) == MAP_SHARED )
		{
			pwrite( *get_file(), (const char*) addr, len, its_offset );
		}
	}
	
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
		StreamHandle( flags, methods ),
		itsMark()
	{
	}
	
	RegularFileHandle::RegularFileHandle( const vfs::node_ptr&               file,
	                                      int                                flags,
	                                      const vfs::filehandle_method_set*  methods )
	:
		StreamHandle( file.get(), flags, methods ),
		itsMark()
	{
	}
	
	RegularFileHandle::~RegularFileHandle()
	{
	}
	
	ssize_t RegularFileHandle::Append( const char* buffer, size_t n_bytes )
	{
		set_mark( geteof( *this ) );
		
		return pwrite( *this, buffer, n_bytes, get_mark() );
	}
	
	ssize_t RegularFileHandle::SysRead( char* buffer, size_t n_bytes )
	{
		ssize_t read = pread( *this,
		                      buffer,
		                      n_bytes,
		                      get_mark() );
		
		return advance_mark( read );
	}
	
	ssize_t RegularFileHandle::SysWrite( const char* buffer, size_t n_bytes )
	{
		const bool appending = GetFlags() & O_APPEND;
		
		ssize_t written = appending ? Append( buffer, n_bytes )
		                            : pwrite( *this,
		                                      buffer,
		                                      n_bytes,
		                                      get_mark() );
		
		return advance_mark( written );
	}
	
	off_t RegularFileHandle::Seek( off_t offset, int whence )
	{
		off_t base = 0;
		
		switch ( whence )
		{
			case SEEK_SET:
				base = 0;
				break;
			
			case SEEK_CUR:
				base = get_mark();
				break;
			
			case SEEK_END:
				base = geteof( *this );
				break;
			
			default:
				p7::throw_errno( EINVAL );
		}
		
		return set_mark( base + offset );
	}
	
	ssize_t RegularFileHandle::Write( const char* buffer, std::size_t byteCount )
	{
		return SysWrite( buffer, byteCount );
	}
	
	memory_mapping_ptr
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
		
		memory_mapping_ptr result( mapping );
		
		result = memory_mapping_ptr( new file_memory_mapping( mapping, this, offset ) );
		
		void* addr = mapping->get_address();
		
		ssize_t count = pread( *this, (char*) addr, length, offset );
		
		if ( count < length )
		{
			memset( (char*) addr + count, '\0', length - count );
		}
		
		return result;
	}
	
}

