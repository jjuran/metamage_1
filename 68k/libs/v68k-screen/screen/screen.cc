/*
	screen.cc
	---------
*/

#include "screen/screen.hh"

// POSIX
#include <fcntl.h>
#include <unistd.h>

// Standard C
#include <errno.h>

// raster
#include "raster/clut.hh"
#include "raster/load.hh"
#include "raster/sync.hh"

// v68k-record
#include "record/graphics.hh"

// v68k-screen
#include "screen/shared_memory.hh"
#include "screen/storage.hh"
#include "screen/surface.hh"
#include "screen/update.hh"


namespace v68k   {
namespace screen {

using raster::raster_load;
using raster::raster_metadata;
using raster::sync_relay;

using v68k::screen::virtual_buffer;
using v68k::screen::transit_buffer;
using v68k::screen::the_screen_size;
using v68k::screen::the_sync_relay;


uint8_t* page_1_virtual_buffer;
uint8_t* page_2_virtual_buffer;

uint8_t* page_1_transit_buffer;
uint8_t* page_2_transit_buffer;

raster::clut_data* virtual_clut;
raster::clut_data* transit_clut;

static raster_metadata* meta;

static void* spare_virtual_buffer;
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
	
	if ( page_2_virtual_buffer )
	{
		if ( ! spare_virtual_buffer )
		{
			spare_virtual_buffer = page_2_virtual_buffer;
		}
		
		void* old_addr = virtual_buffer;
		void* new_addr = spare_virtual_buffer;
		
		virtual_buffer       = new_addr;
		spare_virtual_buffer = old_addr;
	}
	
	v68k::screen::update_bits();
}

static
void close_without_errno( int fd )
{
	const int saved_errno = errno;
	
	close( fd );
	
	errno = saved_errno;
}

int set_screen_backing_store_file( const char* path )
{
	int fd = open( path, O_RDWR );
	
	if ( fd < 0 )
	{
		return errno;
	}
	
	using namespace raster;
	
	raster_load raster = drive_raster( fd );
	
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
	the_surface_shape.weight = raster.meta->desc.weight;
	
	the_screen_size = raster.meta->desc.height
	                * raster.meta->desc.stride;
	
	record::screen_size( the_surface_shape.width,
	                     the_surface_shape.height,
	                     the_surface_shape.stride,
	                     the_surface_shape.weight );
	
	if ( raster.meta->desc.extra )
	{
		page_2_transit_buffer = page_1_transit_buffer + the_screen_size;
		
		spare_transit_buffer = page_2_transit_buffer;
	}
	
	the_sync_relay = find_sync( &meta->note );
	transit_clut   = find_clut( &meta->note );
	
	return 0;
}

}  // namespace screen
}  // namespace v68k
