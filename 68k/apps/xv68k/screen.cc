/*
	screen.cc
	---------
*/

#include "screen.hh"

// POSIX
#include <fcntl.h>
#include <unistd.h>

// Standard C
#include <errno.h>
#include <string.h>

// raster
#include "raster/load.hh"

// v68k-screen
#include "screen/lock.hh"
#include "screen/storage.hh"
#include "screen/surface.hh"
#include "screen/update.hh"

// xv68k
#include "shared_memory.hh"


using raster::raster_load;
using raster::raster_metadata;
using raster::sync_relay;

using v68k::screen::transit_buffer;
using v68k::screen::the_screen_size;
using v68k::screen::the_sync_relay;


uint8_t* page_1_transit_buffer;
uint8_t* page_2_transit_buffer;

static raster_metadata* meta;

static void* spare_transit_buffer;

static bool using_alternate_buffer;


void page_flip()
{
	using_alternate_buffer = ! using_alternate_buffer;
	
	if ( spare_transit_buffer )
	{
		meta->desc.frame = using_alternate_buffer;
		
		void* old_addr = transit_buffer;
		void* new_addr = spare_transit_buffer;
		
		transit_buffer       = new_addr;
		spare_transit_buffer = old_addr;
	}
	
	v68k::screen::update();
}

static
void close_without_errno( int fd )
{
	const int saved_errno = errno;
	
	close( fd );
	
	errno = saved_errno;
}

static
sync_relay& initialize( raster_load& raster, uint32_t raster_size )
{
	using namespace raster;
	
	raster.meta = (raster_metadata*) ((char*) raster.addr + raster_size);
	
	raster_metadata& meta = *raster.meta;
	
	raster_desc& desc = meta.desc;
	raster_note& note = meta.note;
	
	return data< sync_relay >( note );
}

int set_screen_backing_store_file( const char* path )
{
	int fd = open( path, O_RDWR );
	
	if ( fd < 0 )
	{
		return errno;
	}
	
	using namespace raster;
	
	raster_load raster = play_raster( fd );
	
	close_without_errno( fd );
	
	if ( raster.addr == 0 )  // NULL
	{
		return errno;
	}
	
	transit_buffer = raster.addr;
	
	page_1_transit_buffer = (uint8_t*) transit_buffer;
	
	meta = raster.meta;
	
	using v68k::screen::the_surface_shape;
	
	the_surface_shape.width  = raster.meta->desc.width;
	the_surface_shape.height = raster.meta->desc.height;
	the_surface_shape.stride = raster.meta->desc.stride;
	
	the_screen_size = raster.meta->desc.height
	                * raster.meta->desc.stride;
	
	if ( raster.meta->desc.extra )
	{
		page_2_transit_buffer = page_1_transit_buffer + the_screen_size;
		
		spare_transit_buffer = page_2_transit_buffer;
	}
	
	uint32_t count = 1 + raster.meta->desc.extra;
	
	sync_relay& sync = initialize( raster, the_screen_size * count );
	
	the_sync_relay = &sync;
	
	return 0;
}

namespace screen {

static
uint8_t* translate_live( addr_t addr, uint32_t length, fc_t fc, mem_t access )
{
	if ( transit_buffer == 0 )  // NULL
	{
		return 0;  // NULL
	}
	
	using v68k::screen::is_unlocked;
	
	const uint32_t screen_size = the_screen_size;
	
	if ( length > screen_size )
	{
		// The memory access is somehow wider than the buffer is long
		return 0;  // NULL
	}
	
	if ( addr > screen_size - length )
	{
		return 0;  // NULL
	}
	
	uint8_t* p = (uint8_t*) transit_buffer + addr;
	
	if ( access == v68k::mem_update  &&  is_unlocked() )
	{
		v68k::screen::update();
	}
	
	return p;
}

static
uint8_t* translate_spare( addr_t addr, uint32_t length, fc_t fc, mem_t access )
{
	if ( spare_transit_buffer == 0 )  // NULL
	{
		return 0;  // NULL
	}
	
	const uint32_t screen_size = the_screen_size;
	
	if ( length > screen_size )
	{
		// The memory access is somehow wider than the buffer is long
		return 0;  // NULL
	}
	
	if ( addr > screen_size - length )
	{
		return 0;  // NULL
	}
	
	return (uint8_t*) spare_transit_buffer + addr;
}

uint8_t* translate( addr_t addr, uint32_t length, fc_t fc, mem_t access )
{
	return using_alternate_buffer ? translate_spare( addr, length, fc, access )
	                              : translate_live ( addr, length, fc, access );
}

uint8_t* translate2( addr_t addr, uint32_t length, fc_t fc, mem_t access )
{
	return using_alternate_buffer ? translate_live ( addr, length, fc, access )
	                              : translate_spare( addr, length, fc, access );
}

}  // namespace screen
