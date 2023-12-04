/*
	graphics.cc
	-----------
*/

#include "graphics.hh"

// POSIX
#include <sys/uio.h>

// Standard C
#include <assert.h>
#include <stdlib.h>
#include <string.h>

// iota
#include "iota/endian.hh"

// damogran
#include "damogran/pack.hh"

// v68k-record
#include "record/mb32.hh"
#include "record/recording.hh"
#include "record/time.hh"


#define COUNT( array )  (sizeof (array) / sizeof *(array))


namespace v68k   {
namespace record {

static uint32_t image_size;


static
void memxor_long( long* dst, const long* src, size_t n )
{
	while ( n-- > 0 )
	{
		*dst++ ^= *src++;
	}
}

static
const long* find_nonzero( const long* begin, const long* end )
{
	while ( begin < end )
	{
		if ( *begin++ != 0 )
		{
			return --begin;
		}
	}
	
	return NULL;
}

struct size_chunk
{
	uint32_t tag;
	uint32_t size;
	uint16_t width;
	uint16_t height;
	uint16_t stride;
	uint16_t weight;
};

void screen_size( int width, int height, int stride, int weight )
{
	if ( ! recording() )
	{
		return;
	}
	
	image_size = height * stride;
	
	size_chunk size;
	
	size.tag    = mb32( 'S', 'I', 'Z', 'E' );
	size.size   = iota::big_u32( sizeof size );
	size.width  = iota::big_u16( width  );
	size.height = iota::big_u16( height );
	size.stride = iota::big_u16( stride );
	size.weight = iota::big_u16( weight );
	
	append( &size, sizeof size );
}

static
size_t compress( const uint8_t* begin, const uint8_t* end )
{
	// Precondition:  begin and end are even addresses
	
	unsigned long packed_size = damogran::pack_preflight( begin, end );
	
	packed_size += 2;
	
	uint8_t* buffer = (uint8_t*) alloca( packed_size );
	
	uint8_t* mark = damogran::pack( begin, end, buffer );
	
	*mark++ = 0x00;
	*mark++ = 0x00;
	
	assert( mark - buffer == packed_size );
	
	packed_size &= ~0x2;
	
	uint32_t chunk_size = iota::big_u32( 8 + packed_size );
	
	iovec iov[] =
	{
		{ (char*) "XOR ",      4           },
		{ (char*) &chunk_size, 4           },
		{ (char*) buffer,      packed_size },
	};
	
	appendv( iov, COUNT( iov ) );
	
	return packed_size;
}

void frame( const void* buffer )
{
	if ( ! recording() )
	{
		return;
	}
	
	static uint8_t* prev = (uint8_t*) calloc( image_size, 1 );
	
	timestamp();
	
	memxor_long( (long*) prev, (long*) buffer, image_size / sizeof (long) );
	
	const uint8_t* end = prev + image_size;
	
	compress( prev, end );
	
	memcpy( prev, buffer, image_size );
}

}  // namespace record
}  // namespace v68k
