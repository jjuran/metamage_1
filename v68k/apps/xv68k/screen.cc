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

// raster
#include "raster/load.hh"
#include "raster/relay.hh"

// v68k-callbacks
#include "callback/bridge.hh"

// xv68k
#include "shared_memory.hh"


using raster::raster_load;
using raster::sync_relay;


static bool ignoring_screen_locks;

static void* the_screen_buffer;

static sync_relay* the_sync_relay;


struct end_sync
{
	~end_sync()
	{
		if ( the_sync_relay )
		{
			memset( the_screen_buffer, '\xFF', screen_size );
			
			terminate( *the_sync_relay );
		}
	}
};

#ifndef __RELIX__

static end_sync finally_end_sync;

#endif

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
	
	raster.meta = (raster_metadata*) ((char*) raster.addr + screen_size);
	
	raster_metadata& meta = *raster.meta;
	
	raster_desc& desc = meta.desc;
	raster_note& note = meta.note;
	
	return data< sync_relay >( note );
}

static
int publish_raster( const char* path )
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
	
	the_screen_buffer = raster.addr;
	
	sync_relay& sync = initialize( raster );
	
	the_sync_relay = &sync;
	
	return 0;
}

void ignore_screen_locks()
{
	ignoring_screen_locks = true;
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
	
	const bool unlocked = screen_lock_level >= 0  ||  ignoring_screen_locks;
	
	if ( access == v68k::mem_update  &&  unlocked )
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
