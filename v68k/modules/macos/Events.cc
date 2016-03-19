/*
	Events.cc
	---------
*/

#include "Events.hh"

// Mac OS
#ifndef __EVENTS__
#include <Events.h>
#endif
#ifndef __PROCESSES__
#include <Processes.h>
#endif

// POSIX
#include <unistd.h>
#include <sys/select.h>

// splode
#include "splode/splode.hh"

// macos
#include "Region-ops.hh"
#include "Regions.hh"
#include "options.hh"


UInt32 Ticks : 0x016A;
Point  Mouse : 0x0830;


const unsigned long GetNextEvent_throttle = 2;  // minimum ticks between calls

static timeval wait_timeout;

static
timeval timeval_from_ticks( unsigned long ticks )
{
	const long microseconds_per_tick = 1000 * 1000 / 60;
	
	const long seconds      = ticks / 60;
	const long microseconds = ticks % 60 * microseconds_per_tick;
	
	timeval tv = { seconds, microseconds };
	
	return tv;
}

static
bool wait_for_fd( int fd, timeval* timeout )
{
	fd_set readfds;
	FD_ZERO( &readfds );
	FD_SET( fd, &readfds );
	
	const int max_fd = fd;
	
	int selected = select( max_fd + 1, &readfds, NULL, NULL, timeout );
	
	return selected > 0;
}

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
short populate( EventRecord& event, const splode::pointer_event_buffer& buffer )
{
	using namespace splode::modes;
	using namespace splode::pointer;
	using splode::uint8_t;
	
	const uint8_t mode_mask = Command | Shift | Option | Control;
	
	event.modifiers = (buffer.modes & mode_mask) << 8;
	
	const uint8_t action = buffer.attrs & action_mask;
	
	if ( action == 0 )
	{
		event.what = mouseDown;
		
		return mouseUp;
	}
	
	event.what = action + (mouseDown - splode::pointer::down);
	
	return 0;
}

static inline
void SetMouse( const splode::pointer_location_buffer& buffer )
{
	Mouse.h = buffer.x;
	Mouse.v = buffer.y;
}

static
EventKind read_event( int fd, EventRecord* event )
{
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
		ExitToShell();
	}
	
	switch ( len )
	{
		default:
		case 0:
			using splode::null_message_buffer;
			break;
		
		case 1:
			using splode::ascii_synth_buffer;
			
			event->what    = keyDown;
			event->message = ((ascii_synth_buffer*) buffer)->ascii;
			return keyUp;
		
		case 3:
			using splode::pointer_event_buffer;
			
			return populate( *event, *(pointer_event_buffer*) buffer );
		
		case 5:
			using splode::pointer_location_buffer;
			
			SetMouse( *(pointer_location_buffer*) buffer );
			break;
	}
	
	return 0;
}

static
bool get_event( int fd, EventRecord* event )
{
	static EventRecord queued_event;
	
	if ( queued_event.what )
	{
		*event = queued_event;
		
		queued_event.what = 0;
		
		return true;
	}
	
	EventKind kind_to_queue = 0;
	
	memset( event, '\0', sizeof (EventRecord) );
	
	if ( wait_for_fd( fd, &wait_timeout ) )
	{
		kind_to_queue = read_event( fd, event );
	}
	
	wait_timeout = timeval_from_ticks( GetNextEvent_throttle );
	
	event->when  = Ticks;
	event->where = Mouse;
	
	if ( kind_to_queue )
	{
		queued_event = *event;
		
		queued_event.what = kind_to_queue;
	}
	
	return event->what != nullEvent;
}

static inline
asm UInt32 add_pinned( UInt32 a : __D0, UInt32 b : __D1 ) : __D0
{
	ADD.L    D1,D0
	BCC.S    no_carry
	MOVEQ    #-1,D0
no_carry:
}

pascal unsigned char WaitNextEvent_patch( unsigned short  eventMask,
                                          EventRecord*    event,
                                          unsigned long   sleep,
                                          RgnHandle       mouseRgn )
{
	if ( mouseRgn != NULL  &&  EmptyRgn_patch( mouseRgn ) )
	{
		mouseRgn = NULL;
	}
	
	/*
		Time is fleeting.  Keep a local, non-volatile copy of Ticks.
		
		In theory, Ticks could be (future - 1) at the end of the loop and
		advance to (future + 1) before the timeval_from_ticks() call, since
		each load of Ticks makes a fresh call to gettimeofday() (and the
		same concern would apply it if were updated at interrupt time).
	*/
	
	UInt32 now = Ticks;
	
	/*
		Pin the addition of Ticks and sleep.  In the improbable (but very
		possible) event that an instance of macos runs for over a year,
		Ticks may exceed 2^31, at which point adding 0x7FFFFFFF (2^31 - 1)
		will overflow.  This is bad, because it means that WaitNextEvent()
		calls with a sleep argument of 0x7FFFFFFF (which are intended to
		sleep for an arbitrarily long time) will return immediately, and
		thereby (in all likelihood) consume all available CPU for the next
		year or so (until Ticks passes 2^32 - 1 and overflows on its own).
		
		By pinning the addition to 2^32 - 1, busy-polling is limited to
		one tick (1/60 of a second) after two years or so -- which isn't
		bad at all.  (Though you may have other issues if you let Ticks
		overflow.  I strongly don't recommend it...)
	*/
	
	const UInt32 future = add_pinned( now, sleep );
	
	do
	{
		wait_timeout = timeval_from_ticks( future - now );
		
		const bool got = GetNextEvent( eventMask, event );
		
		if ( got  ||  event->what != nullEvent )
		{
			return got;
		}
		
		if ( mouseRgn != NULL  &&  ! PtInRgn_patch( event->where, mouseRgn ) )
		{
			event->what    = osEvt;
			event->message = mouseMovedMessage << 24;
			
			return true;
		}
		
		now = Ticks;
	}
	while ( now < future );
	
	return false;
}

pascal unsigned char GetNextEvent_patch( unsigned short  eventMask,
                                         EventRecord*    event )
{
	return get_event( events_fd, event );
}
