/*
	make_cursor.cc
	--------------
*/

#include "amicus/make_cursor.hh"

// POSIX
#include <fcntl.h>
#include <unistd.h>

// Standard C
#include <errno.h>
#include <stdlib.h>
#include <string.h>

// shared_cursor
#include "shared_cursor/cursor_file.hh"

// rasterlib
#include "raster/load.hh"
#include "raster/size.hh"
#include "raster/skif.hh"

// amicus
#include "amicus/cursor.hh"


namespace amicus
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
	
	const uint32_t width  = 16;
	const uint32_t height = 16;
	const uint32_t weight = 1;
	
	const uint32_t frame_count = 3;
	
	const uint32_t stride     = 2;
	const uint32_t image_size = height * stride;
	const uint32_t raster_size = image_size * frame_count;
	
	const uint32_t footer_size_minimum = sizeof (raster_metadata)
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
	
	memset( raster.addr, '\x00', raster.size );
	
	raster.meta = (raster_metadata*) ((char*) raster.addr + raster_size);
	
	uint32_t* end = (uint32_t*) ((char*) raster.meta + footer_size);
	
	*--end = footer_size;
	*--end = kSKIFFileType;
	
	raster_metadata& meta = *raster.meta;
	
	raster_desc& desc = meta.desc;
	
	memset( &meta, '\0', sizeof meta );
	
	desc.magic  = kSKIFFileType;
	desc.width  = width;
	desc.height = height;
	desc.stride = stride;
	desc.weight = weight;
	desc.model  = Model_monochrome_paint;
	desc.extra  = frame_count - 1;
	
	result = raster;
	
	return 0;
}

cursor_lifetime::cursor_lifetime( const char* cursor_path )
{
	its_path = cursor_path;
	
	if ( cursor_path )
	{
		int errnum = create_raster_file( cursor_path, its_load );
		
		if ( errnum )
		{
			throw cursor_creation_error();
		}
		
		amicus::cursor_state = open_cursor_file( cursor_path );
		
		if ( amicus::cursor_state )
		{
			setenv( "AMS_CURSOR", "hardware", 1 );
		}
	}
}

cursor_lifetime::~cursor_lifetime()
{
	if ( its_path )
	{
		unlink( its_path );
	}
}

}
