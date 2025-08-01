/*
	sound.cc
	--------
*/

#include "sound.hh"

// POSIX
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

// Standard C
#include <stdlib.h>
#include <string.h>

// iota
#include "iota/endian.hh"

// sndpipe
#include "sndpipe/sndpipe.hh"


namespace v68k  {
namespace sound {

int sound_fd = -1;

uint8_t* the_sound_buffer;

const int sndpipe_buffer_size = 8 + 4 + 370;

static uint8_t message_buffer[ sndpipe_buffer_size ];

long send_command( short domain, const void* buffer, uint32_t buffer_length )
{
	/*
		Callers of this function pass a buffer that includes a `mode` field,
		but the wire protocol regards the mode as part of an 8-byte header --
		therefore the length (which covers only the extent) should exclude it.
	*/
	
	uint32_t length = buffer_length - sizeof (short);
	
	length = iota::big_u32( length );
	domain = iota::big_u16( domain );
	
	const int n_iov = 3;
	
	iovec iov[ n_iov ] =
	{
		{ (char*) &length, sizeof length },
		{ (char*) &domain, sizeof domain },
		{ (char*) buffer,  buffer_length },
	};
	
	return writev( sound_fd, iov, n_iov );
}

struct set_audio_level_buffer
{
	short mode;  // 'll'
	
	uint8_t semantics;
	uint8_t level;
};

void set_audio_level( short level )
{
	if ( sound_fd < 0 )
	{
		return;
	}
	
	set_audio_level_buffer buffer;
	
	buffer.mode      = sndpipe::set_loudness_level;  // 'll'
	buffer.semantics = 0;
	buffer.level     = level & 7;
	
	send_command( sndpipe::admin_domain, &buffer, sizeof buffer );
}

static
bool is_silence_and_zeros( const uint8_t* p, short n )
{
	while ( n-- > 0 )
	{
		if ( (*p++ & 0x7f) )
		{
			return false;
		}
	}
	
	return true;
}

struct audio_frame_buffer
{
	short mode;  // 0 (ffMode)
	
	uint16_t rate_high;
	uint16_t rate_low;
	
	uint8_t samples[ 370 ];
};

struct silent_frame_buffer
{
	short mode;  // '__' (a_frame_of_silence)
};

void send_silent_frame()
{
	/*
		The value of a_frame_of_silence is intentionally
		palindromic, so it doesn't need to be byte-swapped.
	*/
	
	silent_frame_buffer buffer = { sndpipe::a_frame_of_silence };
	
	send_command( sndpipe::sound_domain, &buffer, sizeof buffer );
}

static
void sound_update()
{
	if ( sound_fd < 0 )
	{
		return;
	}
	
	audio_frame_buffer buffer;
	
	buffer.mode = 0;  // ffMode
	buffer.rate_high = iota::big_u16( 1 );  // identity playback rate
	buffer.rate_low  = 0;
	
	uint8_t* p = buffer.samples;
	uint8_t* q = the_sound_buffer;
	
	for ( int i = 0;  i < 370;  ++i )
	{
		*p++ = *q;
		
		q += 2;
	}
	
	if ( is_silence_and_zeros( buffer.samples, 370 ) )
	{
		/*
			Lemmings begins sound generation by filling the sound
			buffer with zero bytes, resulting in a nasty click.
			
			Since it's supposed to be silent anyway, prevent the
			issue by replacing the faulty frame (as well as any
			frames filled with actual silence) with a null frame.
		*/
		
		send_silent_frame();
	}
	else
	{
		send_command( sndpipe::sound_domain, &buffer, sizeof buffer );
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
	
	/*
		Enchanted Scepters writes pairs of samples in reverse order,
		so if we wait only until the last sample is written, we'll have
		missed the one before it.  Instead, wait until both of the final
		samples have been written to the buffer.
	*/
	
	static int counter = 2;
	
	const addr_t last_two = buffer_size - 4;
	
	if ( access == v68k::mem_update  &&  addr >= last_two )
	{
		if ( --counter == 0 )
		{
			sound_update();
			
			counter = 2;
		}
	}
	
	return the_sound_buffer + addr;
}

}  // namespace sound
}  // namespace v68k
