/*
	splode.cc
	---------
*/

#include "splode/write-a-splode.hh"

// POSIX
#include <unistd.h>

// iota
#include "iota/endian.hh"

// splode
#include "splode/splode.hh"


namespace splode
{

static
ssize_t write_buffer( int fd, const void* buffer, size_t len )
{
	ssize_t n_written = write( fd, buffer, len );
	
	if ( n_written < 0 )
	{
		return n_written;  // write() error, return -1
	}
	
	const ssize_t n_short = len - n_written;
	
	return n_short ? ~n_short    // short write, return shortfall as value < -1
	               : n_written;  // success, return count of bytes written
}

int send_mouse_moved_event( int fd, short x, short y )
{
	splode::pointer_location_buffer buffer =
	{
		sizeof buffer - 1,
		0,
		iota::big_u16( x ),
		iota::big_u16( y ),
	};
	
	return write_buffer( fd, &buffer.len, sizeof buffer );
}

int send_mouse_event( int fd, Byte modes, Byte attrs )
{
	using namespace splode::modes;
	
	splode::pointer_event_buffer buffer =
	{
		sizeof buffer - 1,
		modes,
		attrs,
		0,  // unspecified device
	};
	
	return write_buffer( fd, &buffer.len, sizeof buffer );
}

int send_key_event( int fd, char c, Byte modes, Byte attrs )
{
	splode::ascii_event_buffer buffer =
	{
		sizeof buffer - 1,
		c,
		modes,
		attrs,
		0,  // unspecified device
	};
	
	return write_buffer( fd, &buffer.len, sizeof buffer );
}

}
