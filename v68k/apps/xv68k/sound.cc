/*
	sound.cc
	--------
*/

#include "sound.hh"

// POSIX
#include <unistd.h>

// Standard C
#include <stdlib.h>
#include <string.h>


namespace sound {

uint8_t* the_sound_buffer;

const int sndpipe_buffer_size = 8 + 4 + 370;

static uint8_t message_buffer[ sndpipe_buffer_size ];

static
int get_sound_fd()
{
	if ( const char* sound_fd_env = getenv( "XV68K_SOUND_FD" ) )
	{
		return atoi( sound_fd_env );
	}
	
	return -1;
}

static const int sound_fd = get_sound_fd();

static
void sound_update()
{
	uint8_t* p = message_buffer;
	
	const size_t size = sndpipe_buffer_size - 8;  // 374
	
	*p++ = 0;
	*p++ = 0;
	*p++ = size >> 8;
	*p++ = (uint8_t) size;
	*p++ = 'K';  // sound domain
	*p++ = 'K';
	*p++ = 0;    // ffMode
	*p++ = 0;
	
	*p++ = 0;
	*p++ = 1;  // identity playback rate
	*p++ = 0;
	*p++ = 0;
	
	uint8_t* q = the_sound_buffer;
	
	for ( int i = 0;  i < 370;  ++i )
	{
		*p++ = *q;
		
		q += 2;
	}
	
	if ( sound_fd > 0 )
	{
		write( sound_fd, message_buffer, sndpipe_buffer_size );
	}
}

uint8_t* translate( addr_t addr, uint32_t length, fc_t fc, mem_t access )
{
	if ( access == v68k::mem_exec )
	{
		return 0;  // NULL
	}
	
	if ( the_sound_buffer == 0 )  // NULL
	{
		the_sound_buffer = (uint8_t*) malloc( buffer_size );
		
		memset( the_sound_buffer, '\x80', buffer_size );
	}
	
	if ( length > buffer_size )
	{
		// The memory access is somehow wider than the buffer is long
		return 0;  // NULL
	}
	
	if ( addr > buffer_size - length )
	{
		return 0;  // NULL
	}
	
	const addr_t last = buffer_size - 2;
	
	if ( access == v68k::mem_update  &&  addr == last )
	{
		sound_update();
	}
	
	return the_sound_buffer + addr;
}

}  // namespace sound
