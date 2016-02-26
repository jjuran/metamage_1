/*
	Events.cc
	---------
*/

#include "Events.hh"

// Mac OS
#ifndef __EVENTS__
#include <Events.h>
#endif

// POSIX
#include <unistd.h>

// macos
#include "options.hh"


UInt32 Ticks : 0x016A;


static inline
ssize_t direct_read( int fd, unsigned char* buffer, size_t n )
{
	return read( fd, buffer, n );
}

static inline
ssize_t normal_read( int fd, unsigned char* buffer, size_t n )
{
	ssize_t result = read( fd, buffer, sizeof (char) );
	
	if ( result > 0 )
	{
		result = read( fd, buffer + 1, buffer[ 0 ] );
		
		if ( result >= 0 )
		{
			++result;
		}
	}
	
	return result;
}

static inline
ssize_t read( int fd, unsigned char* buffer, size_t n, bool direct )
{
	return direct ? direct_read( fd, buffer, n )
	              : normal_read( fd, buffer, n );
}

static
bool read_event( int fd, EventRecord* event )
{
	memset( event, '\0', sizeof (EventRecord) );
	
	unsigned char buffer[ 256 ];
	
	/*
		Reading 0 bytes from a normal stream yields 0, but an eventtap stream
		will fail with EINVAL.
	*/
	
	const bool direct = read( fd, buffer, 0 );
	
	const ssize_t n_read = read( fd, buffer, sizeof buffer, direct );
	
	const ssize_t len = buffer[ 0 ];
	
	if ( 1 + len != n_read )
	{
		return false;
	}
	
	switch ( len )
	{
		default:
		case 0:
			break;
		
		case 1:
			event->what    = keyDown;
			event->message = buffer[ 1 ];
			break;
	}
	
	event->when = Ticks;
	
	return true;
}

pascal unsigned char GetNextEvent_patch( short eventMask, EventRecord* event )
{
	return read_event( events_fd, event );
}
