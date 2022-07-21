/*
	make_raster.cc
	--------------
*/

#include "amicus/make_raster.hh"

// POSIX
#include <fcntl.h>
#include <unistd.h>

// Standard C
#include <errno.h>
#include <string.h>

// rasterlib
#include "raster/load.hh"
#include "raster/relay_detail.hh"
#include "raster/skif.hh"
#include "raster/sync.hh"


namespace amicus
{

static
uint32_t make_stride( uint32_t width, int weight )
{
	if ( weight > 8 )
	{
		return width * (weight / 8);
	}
	
	if ( weight < 8 )
	{
		int pixels_per_byte = 8 / weight;
		
		width += pixels_per_byte - 1;
		width /= pixels_per_byte;
	}
	
	// width is now in bytes
	
	const int alignment = 2;
	const int mask = alignment - 1;
	
	return width + mask & ~mask;
}

static
uint32_t sizeof_raster( uint32_t raster_size )
{
	using namespace raster;
	
	const uint32_t minimum_footer_size = sizeof (raster_metadata)
	                                   + sizeof (raster_note)
	                                   + sizeof (sync_relay)
	                                   + sizeof (uint32_t);
	
	const uint32_t disk_block_size = 512;
	const uint32_t k               = disk_block_size - 1;
	
	// Round up the total file size to a multiple of the disk block size.
	const uint32_t total_size = (raster_size + minimum_footer_size + k) & ~k;
	
	return total_size;
}

static inline
uint32_t sizeof_footer( uint32_t raster_size )
{
	return sizeof_raster( raster_size ) - raster_size;
}

static
void close_without_errno( int fd )
{
	const int saved_errno = errno;
	
	close( fd );
	
	errno = saved_errno;
}

static
int create_raster_file( const char* path, raster::raster_load& result )
{
	using namespace raster;
	
	int fd = open( path, O_RDWR | O_CREAT, 0664 );
	
	if ( fd < 0 )
	{
		return errno;
	}
	
	const uint32_t width  = 512;
	const uint32_t height = 342;
	const uint32_t weight = 1;
	
	const uint32_t frame_count = 2;
	
	const uint32_t stride     = make_stride( width, weight );
	const uint32_t image_size = height * stride;
	const uint32_t raster_size = image_size * frame_count;
	
	int nok = ftruncate( fd, sizeof_raster( raster_size ) );
	
	if ( nok < 0 )
	{
		return errno;
	}
	
	raster_load raster = create_raster( fd );
	
	close_without_errno( fd );
	
	if ( raster.addr == 0 )  // NULL
	{
		return errno;
	}
	
	const uint32_t footer_size = sizeof_footer( raster_size );
	
	memset( raster.addr, '\xFF', raster.size );
	
	raster.meta = (raster_metadata*) ((char*) raster.addr + raster_size);
	
	uint32_t* end = (uint32_t*) ((char*) raster.meta + footer_size);
	
	*--end = footer_size;
	*--end = kSKIFFileType;
	
	raster_metadata& meta = *raster.meta;
	
	raster_desc& desc = meta.desc;
	raster_note& note = meta.note;
	
	memset( &meta, '\0', sizeof meta );
	
	desc.magic  = kSKIFFileType;
	desc.width  = width;
	desc.height = height;
	desc.stride = stride;
	desc.weight = weight;
	desc.model  = Model_monochrome_paint;
	desc.extra  = frame_count - 1;
	
	raster_note* next_note = &note;
	
	next_note->type = Note_sync;
	next_note->size = sizeof (sync_relay);
	
	next_note = next( next_note );
	
	next_note->type = Note_end;
	
	result = raster;
	
	return 0;
}

raster_lifetime::raster_lifetime( const char* raster_path )
{
	its_path = raster_path;
	
	int errnum = create_raster_file( raster_path, its_load );
	
	if ( errnum )
	{
		throw raster_creation_error();
	}
}

raster_lifetime::~raster_lifetime()
{
	unlink( its_path );
}

}
