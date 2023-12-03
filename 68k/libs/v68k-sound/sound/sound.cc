/*
	sound.cc
	--------
*/

#include "sound.hh"

// POSIX
#include <sys/uio.h>
#include <unistd.h>

// Standard C
#include <stdlib.h>
#include <string.h>

// iota
#include "iota/endian.hh"


namespace v68k  {
namespace sound {

int sound_fd = -1;

uint8_t* the_sound_buffer;

const int sndpipe_buffer_size = 8 + 4 + 370;

static uint8_t message_buffer[ sndpipe_buffer_size ];

long send_command( short domain, const void* buffer, long buffer_length )
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

void set_audio_level( short level )
{
	uint8_t* p = message_buffer;
	
	const size_t payload_size = 2;
	const size_t message_size = 8 + payload_size;
	
	*p++ = 0;
	*p++ = 0;
	*p++ = payload_size >> 8;  // 0
	*p++ = payload_size;       // 2
	*p++ = 'J';                // admin domain
	*p++ = 'J';
	*p++ = 'l';                // set_loudness_level
	*p++ = 'l';
	
	*p++ = 0;                  // semantics
	*p++ = level & 7;          // volume
	
	if ( sound_fd > 0 )
	{
		write( sound_fd, message_buffer, message_size );
	}
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
		p = message_buffer + 12;
		
		if ( is_silence_and_zeros( p, 370 ) )
		{
			/*
				Lemmings begins sound generation by filling the sound
				buffer with zero bytes, resulting in a nasty click.
				
				Prevent that by substituting the actual silence byte value.
			*/
			
			memset( p, '\x80', 370 );
		}
		
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
