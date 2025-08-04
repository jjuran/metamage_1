/*
	start_end.cc
	------------
*/

#include "start_end.hh"

// POSIX
#include <fcntl.h>
#include <unistd.h>

// Standard C
#include <errno.h>

// iota
#include "iota/endian.hh"

// v68k-record
#include "record/recording.hh"
#include "record/time.hh"


namespace v68k   {
namespace record {

int start_recording( const char* path )
{
	recording_fd = open( path, O_WRONLY | O_CREAT | O_TRUNC, 0666 );
	
	if ( recording_fd < 0 )
	{
		return errno;
	}
	
	append( "JANX" "\0\0\0\x10" "\0\0\0\0" "\0\0\0\0", 16 );
	
	const uint32_t tick_duration = 1000;  // 1000ns, for microsecond ticks
	
	const uint32_t tick_duration_BE = iota::big_u32( tick_duration );
	
	append( "TICK" "\0\0\0\x0c", 8 );
	append( &tick_duration_BE,   4 );
	
	return 0;
}

int end_recording()
{
	if ( recording_fd < 0 )
	{
		return EBADF;
	}
	
	timestamp();
	
	append( "END." "\0\0\0\x08", 8 );
	
	close( recording_fd );
	
	recording_fd = -1;
	
	return 0;
}

}  // namespace record
}  // namespace v68k
