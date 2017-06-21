/*
	raster.cc
	---------
*/

#include "raster.hh"

// POSIX
#include <fcntl.h>
#include <unistd.h>
#include <sys/uio.h>

// Standard C
#include <errno.h>
#include <stdlib.h>
#include <string.h>

// config
#include "config/setpshared.h"

// more-posix
#include "more/iov_all.hh"
#include "more/perror.hh"

// rasterlib
#include "raster/load.hh"
#include "raster/relay_detail.hh"
#include "raster/sync.hh"


#define PROGRAM  "telecast-recv"


namespace telekine
{

using namespace raster;

static inline
void report_error( const char* path, int err )
{
	more::perror( PROGRAM, path, err );
}

static
sync_relay* open_raster( const char* path, raster_load& raster )
{
	int raster_fd = open( path, O_RDWR );
	
	if ( raster_fd < 0 )
	{
		report_error( path, errno );
		
		exit( 1 );
	}
	
	using namespace raster;
	
	raster = play_raster( raster_fd );
	
	if ( raster.addr == NULL )
	{
		report_error( path, errno );
		
		exit( 1 );
	}
	
	close( raster_fd );
	
	raster_note* sync = find_note( *raster.meta, Note_sync );
	
	if ( ! is_valid_sync( sync ) )
	{
		report_error( path, ENOSYS );
		
		exit( 3 );
	}
	
	return (sync_relay*) data( sync );
}

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
uint32_t sizeof_raster( uint32_t image_size )
{
	using namespace raster;
	
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
uint32_t sizeof_footer( uint32_t image_size )
{
	return sizeof_raster( image_size ) - image_size;
}

static
void close_without_errno( int fd )
{
	const int saved_errno = errno;
	
	close( fd );
	
	errno = saved_errno;
}

static
int create_raster_file( const char* path, const raster_desc& in_desc, raster::raster_load& result )
{
	using namespace raster;
	
	int fd = open( path, O_RDWR | O_CREAT, 0664 );
	
	if ( fd < 0 )
	{
		return errno;
	}
	
	const uint32_t width  = in_desc.width;
	const uint32_t height = in_desc.height;
	const uint32_t weight = in_desc.weight;
	const uint32_t stride = in_desc.stride;
	
	const uint32_t computed_stride = make_stride( width, weight );
	
	if ( stride < computed_stride  ||  stride > computed_stride * 2 )
	{
		return EINVAL;
	}
	
	const uint32_t image_size = height * stride;
	
	int nok = ftruncate( fd, sizeof_raster( image_size ) );
	
	if ( nok < 0 )
	{
		return errno;
	}
	
	result = create_raster( fd );
	
	close_without_errno( fd );
	
	if ( result.addr == 0 )  // NULL
	{
		return errno;
	}
	
	const uint32_t footer_size = sizeof_footer( image_size );
	
	memset( result.addr, '\xFF', result.size );
	
	result.meta = (raster_metadata*) ((char*) result.addr + image_size);
	
	uint32_t* end = (uint32_t*) ((char*) result.meta + footer_size);
	
	*--end = footer_size;
	
	raster_metadata& meta = *result.meta;
	
	raster::raster_desc& desc = meta.desc;
	raster::raster_note& note = meta.note;
	
	memset( &meta, '\0', sizeof meta );
	
	desc.width  = width;
	desc.height = height;
	desc.stride = stride;
	desc.weight = weight;
	desc.model  = in_desc.model;
	
	raster_note* next_note = &note;
	
	next_note->type = Note_sync;
	next_note->size = sizeof (sync_relay);
	
	next_note = next( next_note );
	
	next_note->type = Note_end;
	
	return 0;
}

raster_task::raster_task( const char* path, const raster_desc& desc )
{
	int err = create_raster_file( path, desc, its_raster );
	
	if ( err )
	{
		report_error( path, err );
		exit( 1 );
	}
	
	its_sync = (sync_relay*) data( &its_raster.meta->note );
	
	publish( *its_sync );
}

raster_task::~raster_task()
{
	terminate( *its_sync );
	
	close_raster( its_raster );
}

void raster_task::recv_rect( int fd, const rect_pv16& r ) const
{
	if ( r.height == 0 )
	{
		return;
	}
	
	const raster::raster_desc& desc = its_raster.meta->desc;
	
	char* addr = (char*) its_raster.addr + r.top * desc.stride;
	
	addr += r.left * desc.weight / 8;
	
	const size_t line_size = r.width * desc.weight / 8;
	const size_t tile_size = r.height * line_size;
	
	const size_t n_iovecs = r.height;
	
	iovec* iov = new iovec[ n_iovecs ];
	
	iovec* p = iov;
	
	for ( int i = 0;  i < r.height;  ++i )
	{
		p->iov_base = (void*) addr;
		p->iov_len  = line_size;
		
		++p;
		
		addr += desc.stride;
	}
	
	int err = more::readv_all( fd, iov, n_iovecs );
	
	delete [] iov;
	
	broadcast( *its_sync );
	
	if ( err < 0 )
	{
		report_error( "recv", errno );
		exit( 1 );
	}
}

}
