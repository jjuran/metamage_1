/*
	load.cc
	-------
*/

#include "raster/load.hh"

// POSIX
#ifdef __APPLE__
// This is needed to compile with Mac OS X 10.3's headers.
#include <sys/types.h>
#endif
#include <sys/mman.h>

// Standard C
#include <errno.h>

// iota
#include "iota/endian.hh"

// raster
#include "raster/clut.hh"
#include "raster/clut_detail.hh"
#include "raster/raster.hh"
#include "raster/screen.hh"
#include "raster/validity.hh"


#pragma exceptions off


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
	
	static inline
	const raster_load& invalid_raster()
	{
		errno = EINVAL;
		return null_raster_load();
	}
	
	static
	uint32_t get_footer_size( void* addr, off_t end )
	{
		const off_t size_offset = end - sizeof (uint32_t);
		
		const uint32_t* size_addr = (uint32_t*) ((char*) addr + size_offset);
		
		return *size_addr;
	}
	
	static
	bool swap_bytes( raster_metadata& meta, const void* note_limit )
	{
		raster_desc& desc = meta.desc;
		
		desc.version = iota::swap_4_bytes( desc.version );
		desc.width   = iota::swap_4_bytes( desc.width   );
		desc.height  = iota::swap_4_bytes( desc.height  );
		desc.stride  = iota::swap_4_bytes( desc.stride  );
		
		// weight and model are single bytes
		// extra and frame are single bytes
		// layout contains single bytes (per_pixel can be used un-swapped)
		
		raster_note* note = &meta.note;
		
		while ( exists( note ) )
		{
			note->size = iota::swap_2_bytes( note->size );
			
			if ( note->type == Note_clut )
			{
				clut_data& clut = data< clut_data >( *note );
				
				clut.seed  = iota::swap_4_bytes( clut.seed  );
				clut.flags = iota::swap_2_bytes( clut.flags );
				
				for ( int i = 0;  i <= clut.max;  ++i )
				{
					color& c = clut.palette[ i ];
					
					c.value = iota::swap_2_bytes( c.value );
					c.red   = iota::swap_2_bytes( c.red   );
					c.green = iota::swap_2_bytes( c.green );
					c.blue  = iota::swap_2_bytes( c.blue  );
				}
			}
			
			note = next( note );
			
			if ( note > (const raster_note*) note_limit )
			{
				return false;
			}
		}
		
		return true;
	}
	
	static
	bool walk_notes( const raster_metadata& meta, const void* note_limit )
	{
		const raster_note* note = &meta.note;
		
		while ( exists( note ) )
		{
			note = next( note );
			
			if ( note > (const raster_note*) note_limit )
			{
				return false;
			}
		}
		
		return true;
	}
	
	
	raster_load open_raster( void* addr, long end, bool synchronized )
	{
		const bool byte_swapping_allowed = ! synchronized;
		
		const size_t file_size = end;
		
		if ( file_size == mac_screen_size )
		{
			return mac_screen_raster_load( addr );
		}
		
		uint32_t footer_size = get_footer_size( addr, end );
		
		bool byte_swapped = false;
		
		if ( footer_size > 0xFFFF )
		{
			if ( ! byte_swapping_allowed  ||  (uint16_t) footer_size != 0 )
			{
				return invalid_raster();
			}
			
			footer_size = iota::swap_4_bytes( footer_size );
			
			byte_swapped = true;
		}
		
		if ( ! is_valid_footer_size( footer_size, end ) )
		{
			return invalid_raster();
		}
		
		const off_t footer_offset = end - footer_size;
		
		void* const footer_addr = (char*) addr + footer_offset;
		
		raster_metadata* const meta = (raster_metadata*) footer_addr;
		
		const void* note_limit = (char*) addr + end
		                                      - sizeof (uint32_t)
		                                      - sizeof (raster_note);
		
		const bool checked = byte_swapped ? swap_bytes( *meta, note_limit )
		                                  : walk_notes( *meta, note_limit );
		
		if ( ! checked )
		{
			return invalid_raster();
		}
		
		if ( ! is_valid_metadata( footer_offset, meta->desc ) )
		{
			return invalid_raster();
		}
		
		raster_load result = { addr, file_size, meta };
		
		return result;
	}
	
	raster_load open_raster( int fd, bool synchronized )
	{
		const bool mac_screens_allowed   = ! synchronized;
		const bool byte_swapping_allowed = ! synchronized;
		
		const off_t end = size_of_file_if_valid( fd );
		
		if ( end < 0 )
		{
			return null_raster_load();
		}
		
		const size_t file_size = end;
		
		if ( file_size == mac_screen_size  &&  ! mac_screens_allowed )
		{
			return invalid_raster();
		}
		
		const int mmap_prot  = PROT_READ | PROT_WRITE;
		const int mmap_flags = synchronized ? MAP_SHARED : MAP_PRIVATE;
		
		void* const addr = mmap( NULL, end, mmap_prot, mmap_flags, fd, 0 );
		
		if ( addr == MAP_FAILED )
		{
			return null_raster_load();
		}
		
		mmap_box box( addr, file_size );
		
		raster_load result = open_raster( addr, end, synchronized );
		
		box.release();
		
		return result;
	}
	
	raster_load create_raster( int fd )
	{
		const off_t end = size_of_file_if_valid( fd );
		
		if ( end < 0 )
		{
			return null_raster_load();
		}
		
		const size_t file_size = end;
		
		const int mmap_prot  = PROT_READ | PROT_WRITE;
		const int mmap_flags = MAP_SHARED;
		
		void* const addr = mmap( NULL, end, mmap_prot, mmap_flags, fd, 0 );
		
		if ( addr == MAP_FAILED )
		{
			return null_raster_load();
		}
		
		raster_load result = { addr, file_size, NULL };
		
		return result;
	}
	
	void close_raster( raster_load& raster )
	{
		munmap( raster.addr, raster.size );
	}
	
}
