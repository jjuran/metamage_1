/*
	screen.cc
	---------
*/

#include "screen.hh"

// POSIX
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

// Standard C
#include <errno.h>
#include <string.h>

// jack
#include "jack/fifo.hh"
#include "jack/interface.hh"

// raster
#include "raster/load.hh"
#include "raster/relay.hh"
#include "raster/relay_detail.hh"
#include "raster/sync.hh"

// v68k-callbacks
#include "callback/bridge.hh"

// xv68k
#include "shared_memory.hh"


using raster::raster_load;
using raster::sync_relay;


static void* the_screen_buffer;

static sync_relay* the_sync_relay;


static inline
uint32_t sizeof_raster()
{
	using namespace raster;
	
	const uint32_t image_size = screen_size;  // 21888
	
	const uint32_t minimum_footer_size = sizeof (raster_metadata)
	                                   + sizeof (raster_note)
	                                   + sizeof (sync_relay)
	                                   + sizeof (uint32_t);
	
	const uint32_t disk_block_size = 512;
	const uint32_t k               = disk_block_size - 1;
	
	// Round up the total file size to a multiple of the disk block size.
	const uint32_t total_size = (image_size + minimum_footer_size + k) & ~k;
	
	return total_size;
}

static inline
uint32_t sizeof_footer()
{
	const uint32_t image_size = screen_size;  // 21888
	
	return sizeof_raster() - image_size;
}

static
void close_without_errno( int fd )
{
	const int saved_errno = errno;
	
	close( fd );
	
	errno = saved_errno;
}

static
sync_relay& initialize( raster_load& raster )
{
	using namespace raster;
	
	const uint32_t footer_size = sizeof_footer();
	
	memset( raster.addr, '\xFF', raster.size );
	
	raster.meta = (raster_metadata*) ((char*) raster.addr + screen_size);
	
	uint32_t* end = (uint32_t*) ((char*) raster.meta + footer_size);
	
	*--end = footer_size;
	
	raster_metadata& meta = *raster.meta;
	
	raster_desc& desc = meta.desc;
	raster_note& note = meta.note;
	
	memset( &meta, '\0', sizeof meta );
	
	desc.width  = 512;
	desc.height = 342;
	desc.stride = 64;
	desc.weight = 1;
	
	note.type = Note_sync;
	note.size = sizeof (sync_relay);
	
	raster_note& last = next( note );
	
	last.type = Note_end;
	
	return data< sync_relay >( note );
}

static
int publish_raster( const char* path )
{
	int fd = open( path, O_RDWR | O_CREAT, 0664 );
	
	if ( fd < 0 )
	{
		return errno;
	}
	
	int nok = ftruncate( fd, sizeof_raster() );
	
	if ( nok < 0 )
	{
		return errno;
	}
	
	using namespace raster;
	
	raster_load raster = create_raster( fd );
	
	close_without_errno( fd );
	
	if ( raster.addr == 0 )  // NULL
	{
		return errno;
	}
	
	the_screen_buffer = raster.addr;
	
	sync_relay& sync = initialize( raster );
	
	publish( sync );
	
	the_sync_relay = &sync;
	
	jack::interface ji = path;
	
	const char* fifo_path = ji.fifo_path();
	
	int fifo_fd = jack::fifo_ready( fifo_path, 0622 );
	
	if ( fifo_fd < 0 )
	{
		return errno;
	}
	
	return 0;
}

int set_screen_backing_store_file( const char* path, bool is_raster )
{
	if ( is_raster )
	{
	#ifdef __RELIX__
		
		return ENOSYS;
		
	#endif
		
		return publish_raster( path );
	}
	
	the_screen_buffer = open_shared_memory( path, screen_size );
	
	if ( the_screen_buffer == 0 )  // NULL
	{
		return errno;
	}
	
	return 0;
}

uint8_t* screen_memory::translate( uint32_t               addr,
                                   uint32_t               length,
                                   v68k::function_code_t  fc,
                                   v68k::memory_access_t  access ) const
{
	if ( access == v68k::mem_exec )
	{
		return 0;  // NULL
	}
	
	if ( the_screen_buffer == 0 )  // NULL
	{
		return 0;  // NULL
	}
	
	using v68k::callback::screen_lock_level;
	
	if ( length > screen_size )
	{
		// The memory access is somehow wider than the buffer is long
		return 0;  // NULL
	}
	
	if ( addr > screen_size - length )
	{
		return 0;  // NULL
	}
	
	uint8_t* p = (uint8_t*) the_screen_buffer + addr;
	
	if ( access == v68k::mem_update  &&  screen_lock_level >= 0 )
	{
	#ifdef __RELIX__
		
		msync( the_screen_buffer, screen_size, MS_SYNC );
		
	#else
		
		if ( the_sync_relay != NULL )
		{
			raster::broadcast( *the_sync_relay );
		}
		
	#endif
	}
	
	return p;
}
