/*
	load.cc
	-------
*/

#include "raster/load.hh"

// POSIX
#include <sys/mman.h>

// Standard C
#include <errno.h>

// iota
#include "iota/endian.hh"

// raster
#include "raster/raster.hh"
#include "raster/screen.hh"
#include "raster/validity.hh"


namespace raster
{
	
	class mmap_box
	{
		private:
			void*   its_addr;
			size_t  its_size;
			
			// non-copyable
			mmap_box           ( const mmap_box& );
			mmap_box& operator=( const mmap_box& );
		
		public:
			mmap_box( void* addr, size_t n ) : its_addr( addr ), its_size( n )
			{
			}
			
			~mmap_box()
			{
				if ( its_addr )
				{
					int saved_errno = errno;
					
					munmap( its_addr, its_size );
					
					errno = saved_errno;
				}
			}
			
			void release()
			{
				its_addr = NULL;
			}
	};
	
	static
	const raster_load& null_raster_load()
	{
		static raster_load null = { 0 };
		
		return null;
	}
	
	static
	uint32_t get_footer_size( void* addr, off_t end )
	{
		const off_t size_offset = end - sizeof (uint32_t);
		
		const uint32_t* size_addr = (uint32_t*) ((char*) addr + size_offset);
		
		return *size_addr;
	}
	
	static
	void swap_bytes( raster_desc& desc )
	{
		desc.magic   = iota::swap_4_bytes( desc.magic   );
		desc.version = iota::swap_4_bytes( desc.version );
		desc.width   = iota::swap_4_bytes( desc.width   );
		desc.height  = iota::swap_4_bytes( desc.height  );
		desc.stride  = iota::swap_4_bytes( desc.stride  );
		
		// weight and model are single bytes
	}
	
	
	raster_load load_raster( int fd )
	{
		const off_t end = size_of_file_if_valid( fd );
		
		if ( end < 0 )
		{
			return null_raster_load();
		}
		
		const int mmap_prot  = PROT_READ | PROT_WRITE;
		const int mmap_flags = MAP_PRIVATE;
		
		void* const addr = mmap( NULL, end, mmap_prot, mmap_flags, fd, 0 );
		
		if ( addr == MAP_FAILED )
		{
			return null_raster_load();
		}
		
		if ( end == mac_screen_size )
		{
			return mac_screen_raster_load( addr );
		}
		
		mmap_box box( addr, end );
		
		uint32_t footer_size = get_footer_size( addr, end );
		
		bool byte_swapped = false;
		
		if ( footer_size > 0xFFFF )
		{
			if ( (uint16_t) footer_size != 0 )
			{
				errno = EINVAL;
				return null_raster_load();
			}
			
			footer_size = iota::swap_4_bytes( footer_size );
			
			byte_swapped = true;
		}
		
		if ( ! is_valid_footer_size( footer_size, end ) )
		{
			errno = EINVAL;
			return null_raster_load();
		}
		
		const off_t footer_offset = end - footer_size;
		
		void* const footer_addr = (char*) addr + footer_offset;
		
		raster_metadata* const meta = (raster_metadata*) footer_addr;
		
		if ( byte_swapped )
		{
			swap_bytes( meta->desc );
		}
		
		if ( ! is_valid_metadata( footer_offset, meta->desc ) )
		{
			errno = EINVAL;
			return null_raster_load();
		}
		
		box.release();
		
		raster_load result = { addr, end, meta };
		
		return result;
	}
	
	void unload_raster( raster_load& loaded )
	{
		munmap( loaded.addr, loaded.size );
	}
	
}
