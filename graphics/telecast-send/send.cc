/*
	send.cc
	-------
*/

#include "send.hh"

// POSIX
#include <unistd.h>
#include <sys/uio.h>

// Standard C
#include <errno.h>
#include <stdlib.h>

// iota
#include "iota/endian.hh"

// more-posix
#include "more/perror.hh"

// telekine
#include "telekine/commands.hh"
#include "telekine/types.hh"


#define PROGRAM  "telecast-send"

#define ARRAY_LEN( a )  (sizeof a / sizeof a[0])


namespace telekine
{

namespace p7 = poseven;


static inline
void report_error( const char* path, int err )
{
	more::perror( PROGRAM, path, err );
}

void* send_task::wait_start( void* arg )
{
	send_task* that = (send_task*) arg;
	
	return NULL;
}

send_task::send_task( int in, int out, p7::mutex& mutex, p7::cond& cond )
:
	its_ready_mutex( mutex ),
	its_ready_cond ( cond  )
{
	its_in_fd  = in;
	its_out_fd = out;
	
	//its_thread.create( &wait_start, this );
}

send_task::~send_task()
{
	if ( its_thread.exists() )
	{
		its_thread.cancel();
		its_thread.join();
	}
}

void send_task::send_desc( const raster::raster_desc& desc ) const
{
	const uint32_t size_BE = iota::big_u32( sizeof (raster_desc) );
	
	const command_header command = { Command_desc, size_BE };
	
	raster_desc desc_BE = { 0 };
	
	desc_BE.width  = iota::big_u32( desc.width  );
	desc_BE.height = iota::big_u32( desc.height );
	desc_BE.stride = iota::big_u32( desc.stride );
	desc_BE.weight = desc.weight;
	desc_BE.model  = desc.model;
	
	iovec iov[] =
	{
		{ (void*) &command, sizeof command },
		{ (void*) &desc_BE, sizeof desc_BE },
	};
	
	writev( its_out_fd, iov, ARRAY_LEN( iov ) );
}

}
