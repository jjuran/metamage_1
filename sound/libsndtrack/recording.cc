/*
	recording.cc
	------------
*/

#include "recording.hh"

// POSIX
#include <fcntl.h>
#include <unistd.h>

// Standard C
#include <stddef.h>
#include <stdlib.h>

// iota
#include "iota/endian.hh"


static inline
uint32_t mb32_native_endian( uint8_t a, uint8_t b, uint8_t c, uint8_t d )
{
	return (a << 24) | (b << 16) | (c << 8) | d;
}

static inline
uint32_t mb32( uint8_t a, uint8_t b, uint8_t c, uint8_t d )
{
	return iota::is_little_endian() ? mb32_native_endian( d, c, b, a )
	                                : mb32_native_endian( a, b, c, d );
}

struct wav_header
{
	uint32_t RIFF;
	uint32_t size;
	uint32_t WAVE;
	
	uint32_t fmt;
	uint32_t flen;
	uint16_t type;
	uint16_t chan;
	uint32_t rate;  // sample rate
	uint32_t Bps;
	uint16_t Bpf;
	uint16_t BpS;
	
	uint32_t data;
	uint32_t dlen;
};

enum
{
	fmt_len = offsetof( wav_header, data ) - offsetof( wav_header, type ),
	
	sample_rate = 22254,
	n_channels = 1,
	bits_per_sample = 16,
	bytes_per_frame = bits_per_sample * n_channels / 8,
	bytes_per_second = bytes_per_frame * sample_rate,
};

static wav_header header =
{
	mb32( 'R', 'I', 'F', 'F' ),            // 'RIFF'
	0,
	mb32( 'W', 'A', 'V', 'E' ),            // 'WAVE'
	
	mb32( 'f', 'm', 't', ' ' ),            // 'fmt '
	iota::little_u32( fmt_len ),           // 16 bytes
	iota::little_u16( 1 ),                 // PCM-coded
	iota::little_u16( n_channels ),        // 1
	iota::little_u32( sample_rate ),       // 22254
	iota::little_u32( bytes_per_second ),  // 22254 * 2
	iota::little_u16( bytes_per_frame ),   // 2
	iota::little_u16( bits_per_sample ),   // 16
	
	mb32( 'd', 'a', 't', 'a' ),            // 'data'
	0,
};

static int recording_fd = -1;

static bool live;               // Initial silence broken?
static long length_of_silence;  // Byte size of most recent silence (if live)

static inline
void write_header()
{
	write( recording_fd, &header, sizeof header );
}

static inline
bool is_recording()
{
	return recording_fd >= 0;
}

void start_recording( const char* path )
{
	recording_fd = open( path, O_WRONLY | O_CREAT | O_EXCL, 0666 );
	
	write_header();
}

static inline
bool is_nonzero( const char* data, unsigned size )
{
	while ( size-- )
	{
		if ( *data++ != '\0' )
		{
			return true;
		}
	}
	
	return false;
}

static inline
bool is_live( const void* data, unsigned size )
{
	return is_nonzero( (const char*) data, size );
}

void append_to_recording( const void* data, unsigned size )
{
	/*
		Note:  In this context, we're using "packet"
		to mean a buffer's worth of audio frames.
		
		A "live" packet is one that doesn't consist entirely
		of silence.  The recording generally is considered
		"live" once a live packet has been received.
		
		We initially discard silent audio packets.  Once the
		recording goes live, we record silence but track the
		length of the current stretch of it.  At the end of
		the recording, any trailing silence is truncated.
		
		Silent packets contain only zero bytes and don't need
		to be byte-swapped.
	*/
	
	bool live_packet;
	
	if ( is_recording()  &&  ((live_packet = is_live( data, size ))  ||  live) )
	{
		if ( live_packet )
		{
			live = true;
			
			length_of_silence = 0;
		}
		else
		{
			length_of_silence += size;
		}
		
		if ( ! iota::is_little_endian()  &&  live_packet )
		{
			uint16_t const* p = (uint16_t const*) data;
			uint16_t      * q = (uint16_t      *) alloca( size );
			
			data = q;
			
			unsigned n = size / 2;
			
			for ( int i = 0;  i < n;  ++i )
			{
				*q++ = iota::little_u16( *p++ );
			}
		}
		
		write( recording_fd, data, size );
		
		header.dlen += size;
	}
}

void end_recording()
{
	if ( is_recording() )
	{
		if ( length_of_silence  &&  length_of_silence < header.dlen )
		{
			header.dlen -= length_of_silence;
			
			ftruncate( recording_fd, header.dlen );
		}
		
		header.size = iota::little_u32( sizeof header + header.dlen );
		header.dlen = iota::little_u32( header.dlen );
		
		lseek( recording_fd, 0, SEEK_SET );
		
		write_header();
	}
}
