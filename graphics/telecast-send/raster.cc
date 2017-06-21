/*
	raster.cc
	---------
*/

#include "raster.hh"

#include <stdio.h>

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

// telekine
#include "telekine/commands.hh"


#define PROGRAM  "telecast-send"

#define STR_LEN( s )  "" s, (sizeof s - 1)

#define POLLING_ENSUES  \
	"WARNING: pthread_cond_wait() is broken -- will poll every 10ms instead"

#define WARN( msg )  write( STDERR_FILENO, STR_LEN( PROGRAM ": " msg "\n" ) )


namespace telekine
{

namespace p7 = poseven;

using namespace raster;

static inline
void report_error( const char* path, int err )
{
	more::perror( PROGRAM, path, err );
}

rect_pv16 buffer_diff_rect( char* one, char* two, raster::raster_desc& desc )
{
	rect_pv16 result = { 0 };
	
	const size_t image_size = desc.height * desc.stride;
	
	const char* p = one;
	const char* q = two;
	
	uint16_t top    = 0;
	uint16_t bottom = desc.height;
	
	while ( top < bottom )
	{
		if ( memcmp( p, q, desc.stride ) != 0 )
		{
			break;
		}
		
		p += desc.stride;
		q += desc.stride;
		
		++top;
	}
	
	if ( top == bottom )
	{
		return result;  // Nothing changed, return empty rect
	}
	
	result.top = top;
	
	p = one + image_size;
	q = two + image_size;
	
	while ( bottom > top )
	{
		p -= desc.stride;
		q -= desc.stride;
		
		if ( memcmp( p, q, desc.stride ) != 0 )
		{
			break;
		}
		
		--bottom;
	}
	
	uint16_t left_margin_bytes = desc.stride;
	
	for ( int i = top;  i < bottom;  ++i )
	{
		p = one + i * desc.stride;
		q = two + i * desc.stride;
		
		for ( int j = 0;  j < left_margin_bytes;  ++j )
		{
			if ( *p++ != *q++ )
			{
				left_margin_bytes = j;
				break;
			}
		}
		
		if ( left_margin_bytes == 0 )
		{
			break;
		}
	}
	
	const uint16_t left = left_margin_bytes * 8 / desc.weight;
	
	result.left = left;
	
	uint16_t right_edge_bytes = 0;
	
	for ( int i = bottom;  i > top;  --i )
	{
		p = one + i * desc.stride;
		q = two + i * desc.stride;
		
		for ( int j = desc.stride;  j > right_edge_bytes;  --j )
		{
			if ( *--p != *--q )
			{
				right_edge_bytes = j;
				break;
			}
		}
		
		if ( right_edge_bytes == desc.stride )
		{
			break;
		}
	}
	
	uint16_t right_margin_bytes = desc.stride - right_edge_bytes;
	
	uint16_t right = desc.width - right_margin_bytes * 8 / desc.weight;
	
	result.height = bottom - top;
	result.width  = right - left;
	
	return result;
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
		return NULL;
		
		report_error( path, ENOSYS );
		
		exit( 3 );
	}
	
	return (sync_relay*) data( sync );
}

rect_pv16 raster_task::get_rect() const
{
	return buffer_diff_rect( (char*) its_raster.addr,
	                         its_buffer,
	                         its_raster.meta->desc );
}

void raster_task::copy_bits() const
{
	const raster::raster_desc& desc = its_raster.meta->desc;
	
	const size_t image_size = desc.height * desc.stride;
	
	memcpy( its_buffer, its_raster.addr, image_size );
}

void raster_task::ready()
{
	p7::lock k( its_ready_mutex );
	
	its_ready_cond.broadcast();
}

void* raster_task::wait_start( void* arg )
{
	raster_task* that = (raster_task*) arg;
	
	const char* feed_path = that->its_feed_path;
	int feed_fd           = that->its_feed_fd;
	
	sync_relay* sync = that->its_sync;
	
	uint32_t seed = 0;
	
	bool can_wait = CONFIG_SETPSHARED;
	
	while ( sync->status == Sync_ready )
	{
		while ( seed == sync->seed )
		{
			if ( feed_fd >= 0 )
			{
				char c;
				ssize_t n = read( feed_fd, &c, sizeof c );
				
				if ( n > 0 )
				{
					continue;
				}
				
				if ( n < 0 )
				{
					report_error( feed_path, errno );
				}
				
				break;
			}
			else if ( can_wait )
			{
				try
				{
					raster::wait( *sync );
				}
				catch ( const wait_failed& )
				{
					WARN( POLLING_ENSUES );
					
					can_wait = false;
					
					continue;
				}
			}
			else
			{
				usleep( 10000 );  // 10ms
			}
			
			p7::thread::testcancel();
		}
		
		seed = sync->seed;
		
		that->ready();
	}
	
	return NULL;
}

raster_task::raster_task( const char*  path,
                          p7::mutex&   mutex,
                          p7::cond&    cond,
                          const char*  feed_path,
                          int          feed_fd )
:
	its_ready_mutex( mutex ),
	its_ready_cond ( cond  ),
	its_feed_path  ( feed_path ),
	its_feed_fd    ( feed_fd   )
{
	its_sync = open_raster( path, its_raster );
	
	const raster::raster_desc& desc = its_raster.meta->desc;
	
	const size_t image_size = desc.height * desc.stride;
	
	its_buffer = (char*) ::operator new( image_size );
	
	if ( its_sync == 0 )  return;
	
	its_thread.create( &wait_start, this );
}

raster_task::~raster_task()
{
	if ( its_thread.exists() )
	{
		its_thread.cancel( &its_sync->cond );
		its_thread.join();
	}
	
	::operator delete( its_buffer );
}

bool raster_task::running() const
{
	return its_sync->status == Sync_ready;
}

struct rect_command
{
	command_header  header;
	rect_pv16       rect;
};

void raster_task::send_rect( int fd, const rect_pv16& rect ) const
{
	if ( rect.height == 0 )
	{
		return;
	}
	
	const raster::raster_desc& desc = its_raster.meta->desc;
	
	const char* addr = (char*) its_raster.addr + rect.top * desc.stride;
	
	addr += rect.left * desc.weight / 8;
	
	const size_t line_size = rect.width * desc.weight / 8;
	const size_t tile_size = rect.height * line_size;
	
	rect_command command = { { Command_rect, sizeof (rect_pv16) }, rect };
	
	convert_big_endian( command.header );
	convert_big_endian( command.rect   );
	
	const size_t n_iovecs = 1 + rect.height;
	
	iovec* iov = new iovec[ n_iovecs ];
	
	iovec* p = iov;
	
	p->iov_base = (void*) &command;
	p->iov_len  = sizeof command;
	
	for ( int i = 0;  i < rect.height;  ++i )
	{
		++p;
		
		p->iov_base = (void*) addr;
		p->iov_len  = line_size;
		
		addr += desc.stride;
	}
	
	int err = more::writev_all( fd, iov, n_iovecs );
	
	delete [] iov;
	
	if ( err < 0 )
	{
		report_error( "send", errno );
		exit( 1 );
	}
}

}
