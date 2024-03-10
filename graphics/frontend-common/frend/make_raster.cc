/*
	make_raster.cc
	--------------
*/

#include "frend/make_raster.hh"

// POSIX
#include <fcntl.h>
#include <unistd.h>

// Standard C
#include <errno.h>
#include <string.h>

// rasterlib
#include "raster/clut.hh"
#include "raster/clut_detail.hh"
#include "raster/default_clut.hh"
#include "raster/load.hh"
#include "raster/relay_detail.hh"
#include "raster/size.hh"
#include "raster/skif.hh"
#include "raster/sync.hh"


namespace frend
{

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
	
	const bool uses_color = false;
	
	const raster_model model = weight > 8 ? Model_RGB
	                         : uses_color ? Model_palette
	                         :              Model_monochrome_paint;
	
	const bool has_palette = model == Model_palette;
	
	const uint32_t frame_count = 2;
	
	const uint32_t stride     = make_stride( width, weight );
	const uint32_t image_size = height * stride;
	const uint32_t raster_size = image_size * frame_count;
	
	const uint32_t footer_size_minimum = sizeof (raster_metadata)
	                                   + sizeof (raster_note)
	                                   + sizeof (sync_relay)
	                                   + sizeof (raster_note) * has_palette
	                                   + sizeof (clut_data)   * has_palette
	                                   + sizeof (uint32_t);
	
	const off_t file_size = good_file_size( raster_size, footer_size_minimum );
	
	int nok = ftruncate( fd, file_size );
	
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
	
	const uint32_t footer_size = file_size - raster_size;
	
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
	desc.model  = model;
	desc.extra  = frame_count - 1;
	
	raster_note* next_note = &note;
	
	next_note->type = Note_sync;
	next_note->size = sizeof (sync_relay);
	
	sync_relay& sync = data< sync_relay >( *next_note );
	
	memset( &sync, '\0', sizeof (sync_relay) );
	
	next_note = next( next_note );
	
	if ( has_palette )
	{
		const short n_colors = 1 << weight;
		
		next_note->type = Note_clut;
		next_note->size = sizeof (clut_header) + sizeof (color) * n_colors;
		
		clut_data& clut = data< clut_data >( *next_note );
		
		load_default_clut( clut, n_colors );
		
		next_note = next( next_note );
	}
	
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
