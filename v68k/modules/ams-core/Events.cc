/*
	Events.cc
	---------
*/

#include "Events.hh"

// Mac OS
#ifndef __EVENTS__
#include <Events.h>
#endif
#ifndef __MACWINDOWS__
#include <MacWindows.h>
#endif
#ifndef __TIMER__
#include <Timer.h>
#endif

// ams-common
#include "callouts.hh"

// ams-core
#include "splode.hh"


UInt32 Ticks   : 0x016A;
QHdr EventQueue : 0x014A;
Byte   MBState : 0x0172;
UInt8  escapes : 0x0173;  // count of Button() calls that won't block
KeyMap KeyMaps : 0x0174;
Point  Mouse   : 0x0830;

WindowRef CurActivate : 0x0A64;
WindowRef CurDeactive : 0x0A68;


const long default_GetNextEvent_throttle = 2;  // minimum ticks between calls

unsigned long GetNextEvent_throttle = default_GetNextEvent_throttle;

static unsigned long next_sleep;

static unsigned long polling_interval = 0;

#pragma mark Accessing Events
#pragma mark -

static
bool get_lowlevel_event( short eventMask, EventRecord* event )
{
	if ( eventMask != autoKeyMask )
	{
		if ( GetOSEvent( eventMask & ~autoKeyMask, event ) )
		{
			button_clicked = false;
			
			return true;
		}
	}
	
	if ( eventMask & autoKeyMask )
	{
		return GetOSEvent( eventMask, event );
	}
	
	return false;
}

pascal unsigned char GetNextEvent_patch( unsigned short  eventMask,
                                         EventRecord*    event )
{
	polling_interval = 0;
	
	UInt32 ticks = get_Ticks();
	
	const unsigned long sleep = next_sleep;
	
	next_sleep = 0;
	
	poll_user_input();
	
	if ( eventMask & activMask )
	{
		if ( CurDeactive )
		{
			event->what      = activateEvt;
			event->message   = (long) CurDeactive;
			event->when      = ticks;
			event->where     = Mouse;
			event->modifiers = 0;
			
			CurDeactive = NULL;
			
			goto got_event;
		}
		
		if ( CurActivate )
		{
			event->what      = activateEvt;
			event->message   = (long) CurActivate;
			event->when      = ticks;
			event->where     = Mouse;
			event->modifiers = activeFlag;
			
			CurActivate = NULL;
			
			goto got_event;
		}
	}
	
	if ( get_lowlevel_event( eventMask, event ) )
	{
		goto got_event;
	}
	
	if ( eventMask & updateMask  &&  CheckUpdate( event ) )
	{
		goto got_event;
	}
	
	wait_for_user_input( sleep );
	
	if ( get_lowlevel_event( eventMask, event ) )
	{
		goto got_event;
	}
	
	/*
		If at any point we return a non-null event, leave next_sleep set to
		zero.  Otherwise, set it to a small but non-zero amount, so we only
		waste a little CPU in applications that call GetNextEvent() instead
		of WaitNextEvent(), rather than consuming one entirely.
	*/
	
	next_sleep = GetNextEvent_throttle;
	
	if ( GetNextEvent_throttle == 0 )
	{
		static UInt32 last_ticks;
		static UInt32 null_events_per_tick;
		
		if ( ticks != last_ticks )
		{
			null_events_per_tick = 0;
			
			last_ticks = ticks;
		}
		else if ( ++null_events_per_tick > 2 )
		{
			next_sleep = default_GetNextEvent_throttle;
		}
	}
	
	EventQueue.qFlags &= ~1;
	
	return false;
	
got_event:
	
	EventQueue.qFlags &= ~1;
	
	return ! SystemEvent( event );
}

static
bool peek_lowlevel_event( short eventMask, EventRecord* event )
{
	if ( eventMask != autoKeyMask )
	{
		if ( OSEventAvail( eventMask & ~autoKeyMask, event ) )
		{
			return true;
		}
	}
	
	if ( eventMask & autoKeyMask )
	{
		return OSEventAvail( eventMask, event );
	}
	
	return false;
}

pascal unsigned char EventAvail_patch( unsigned short  eventMask,
                                       EventRecord*    event )
{
	polling_interval = 0;
	
	const unsigned long sleep = next_sleep;
	
	next_sleep = 0;
	
	poll_user_input();
	
	if ( eventMask & activMask )
	{
		if ( CurDeactive )
		{
			event->what      = activateEvt;
			event->message   = (long) CurDeactive;
			event->when      = get_Ticks();
			event->where     = Mouse;
			event->modifiers = 0;
			
			return true;
		}
		
		if ( CurActivate )
		{
			event->what      = activateEvt;
			event->message   = (long) CurActivate;
			event->when      = get_Ticks();
			event->where     = Mouse;
			event->modifiers = activeFlag;
			
			return true;
		}
	}
	
	if ( peek_lowlevel_event( eventMask, event ) )
	{
		return true;
	}
	
	if ( eventMask & updateMask  &&  CheckUpdate( event ) )
	{
		return true;
	}
	
	wait_for_user_input( sleep );
	
	if ( peek_lowlevel_event( eventMask, event ) )
	{
		return true;
	}
	
	/*
		If at any point we return a non-null event, leave next_sleep set to
		zero.  Otherwise, set it to a small but non-zero amount, so we only
		waste a little CPU in applications that call GetNextEvent() instead
		of WaitNextEvent(), rather than consuming one entirely.
	*/
	
	next_sleep = GetNextEvent_throttle;
	
	return false;
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
	if ( mouseRgn != NULL  &&  EmptyRgn( mouseRgn ) )
	{
		mouseRgn = NULL;
	}
	
	/*
		Time is fleeting.  Keep a local, non-volatile copy of Ticks.
		
		In theory, Ticks could be (future - 1) when compared to future and
		advance to (future + 1) when subtracted from future, since
		each load of Ticks makes a fresh call to gettimeofday() (and the
		same concern would apply it if were updated at interrupt time).
	*/
	
	UInt32 now = get_Ticks();
	
	/*
		Pin sleep to a minimum of 1 if WaitNextEvent() is called within 2ms of
		the previous call.  This shouldn't affect the delivery of non-null
		events, and mitigates core-pegging by applications that pass zero for
		the sleep parameter, which is especially a problem in Lemmings' post-
		gameplay stats screen (which otherwise pegs an entire core).  On the
		other hand, actual gameplay in Lemmings relies on WaitNextEvent() with
		a zero sleep returning immediately.
	*/
	
	if ( sleep == 0 )
	{
		static uint64_t then;
		
		uint64_t now = 0;
		::Microseconds( (UnsignedWide*) &now );
		
		if ( now - then < 2000 )
		{
			sleep = 1;
		}
		
		then = now;
	}
	
	/*
		Pin the addition of Ticks and sleep.  In the improbable (but very
		possible) event that an instance of AMS runs for over a year,
		Ticks may exceed 2^31, at which point adding 0x7FFFFFFF (2^31 - 1)
		will overflow.  This is bad, because it means that WaitNextEvent()
		calls with a sleep argument of 0x7FFFFFFF (which are intended to
		sleep for an arbitrarily long time) will return immediately, and
		thereby (in all likelihood) consume all available CPU for the next
		year or so (until Ticks passes 2^32 - 1 and overflows on its own).
		
		By pinning the addition to 2^32 - 1, busy-polling is limited to
		one tick (1/60.15 of a second) after two years or so -- which isn't
		bad at all.  (Though you may have other issues if you let Ticks
		overflow.  I strongly don't recommend it...)
	*/
	
	const UInt32 future = add_pinned( now, sleep );
	
	/*
		First timeout is zero, so we can process update events.
	*/
	
	next_sleep = 0;
	
	while ( true )
	{
		const bool got = GetNextEvent( eventMask, event );
		
		if ( got  ||  event->what != nullEvent )
		{
			return got;
		}
		
		if ( mouseRgn != NULL  &&  ! PtInRgn( event->where, mouseRgn ) )
		{
			event->what    = osEvt;
			event->message = mouseMovedMessage << 24;
			
			return true;
		}
		
		now = get_Ticks();
		
		if ( now >= future )
		{
			break;
		}
		
		next_sleep = future - now;
	}
	
	return false;
}

#pragma mark -
#pragma mark Reading the Mouse
#pragma mark -

pascal void GetMouse_patch( Point* loc )
{
	poll_user_input();
	
	*loc = Mouse;
	
	GrafPtr thePort;
	GetPort( &thePort );
	
	if ( thePort )
	{
		GlobalToLocal( loc );
	}
}

pascal char Button_patch()
{
	if ( button_clicked )
	{
		button_clicked = false;
		
		return true;
	}
	
	if ( escapes )
	{
		polling_interval = 1;
		
		--escapes;
	}
	
	wait_for_user_input( polling_interval );
	
	polling_interval = 0xFFFFFFFF;
	
	return ! MBState;
}

pascal char StillDown_patch()
{
	EventRecord event;
	
	return Button_patch()  &&  ! OSEventAvail( mDownMask | mUpMask, &event );
}

pascal char WaitMouseUp_patch()
{
	EventRecord event;
	
	return Button_patch()  &&  ! GetOSEvent( mUpMask, &event );
}

pascal long TickCount_patch()
{
	return Ticks;
}

#pragma mark -
#pragma mark Reading the Keyboard and Keypad
#pragma mark -

pascal void GetKeys_patch( KeyMap keys )
{
	UInt32* src = KeyMaps;
	UInt32* dst = keys;
	
	*dst++ = *src++;
	*dst++ = *src++;
	*dst++ = *src++;
	*dst   = *src;
}
