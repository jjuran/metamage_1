// Carbonate/CarbonEvents.cc


#ifndef __CARBONEVENTS__
#include <CarbonEvents.h>
#endif

#if TARGET_API_MAC_CARBON
#error Configuration error:  This file is for classic only
#endif

#if ACCESSOR_CALLS_ARE_FUNCTIONS
// Compile the Carbon accessors as extern pascal functions.
#define CARBONATE_LINKAGE pascal
#include "Carbonate/CarbonEvents.hh"
#endif

#ifndef __TIMER__
#include <Timer.h>
#endif

static const EventRecord gNullEvent = { 0 };

struct OpaqueEventLoopRef
{
	bool quitting;
	
	OpaqueEventLoopRef() : quitting( true )  {}
};

struct OpaqueEventRef
{
	UInt32       retainCount;
	EventRecord  event;
	
	OpaqueEventRef() : retainCount( 1 ), event( gNullEvent )  {}
	
	OpaqueEventRef( const OpaqueEventRef& other ) : retainCount( 1           ),
	                                                event      ( other.event )
	{
	}
	
	private:
		OpaqueEventRef& operator=( const OpaqueEventRef& other );
};

static OpaqueEventLoopRef gMainEventLoop;

// These functions are always declared in the headers and are always extern.

pascal EventLoopRef GetCurrentEventLoop()
{
	return &gMainEventLoop;
}

pascal EventLoopRef GetMainEventLoop()
{
	return &gMainEventLoop;
}

pascal OSStatus RunCurrentEventLoop( EventTimeout timeout )
{
	EventRef ignored;
	
	return ReceiveNextEvent( 0, NULL, timeout, false, &ignored );
}

pascal OSStatus QuitEventLoop( EventLoopRef eventLoop )
{
	if ( eventLoop != &gMainEventLoop )
	{
		return paramErr;
	}
	
	eventLoop->quitting = true;
	
	return noErr;
}

pascal OSStatus ReceiveNextEvent( UInt32                 numTypes,
                                  const EventTypeSpec *  list,
                                  EventTimeout           timeout,
                                  Boolean                pullEvent,
                                  EventRef *             outEvent )
{
	return paramErr;
}

pascal OSStatus MacCreateEvent( CFAllocatorRef    inAllocator,
                                UInt32            inClassID,
                                UInt32            kind,
                                EventTime         when,
                                EventAttributes   flags,
                                EventRef *        outEvent )
{
	try
	{
		*outEvent = new OpaqueEventRef();
	}
	catch ( ... )
	{
		return memFullErr;
	}
	
	return noErr;
}

pascal EventRef CopyEvent( EventRef other )
{
	try
	{
		return new OpaqueEventRef( *other );
	}
	catch ( ... )
	{
		return NULL;
	}
}

pascal EventRef RetainEvent( EventRef event )
{
	++event->retainCount;
	
	return event;
}

pascal UInt32 GetEventRetainCount( EventRef event )
{
	return event->retainCount;
}

pascal void ReleaseEvent( EventRef event )
{
	if ( --event->retainCount == 0 )
	{
		delete event;
	}
}

// SetEventParameter
// GetEventParameter
// GetEventClass
// GetEventKind
// GetEventTime
// SetEventTime

pascal EventQueueRef GetCurrentEventQueue()
{
	return NULL;
}

pascal EventQueueRef GetMainEventQueue()
{
	return NULL;
}

// PostEventToQueue
// FlushEventsMatchingListFromQueue
// FlushSpecificEventsFromQueue

pascal OSStatus FlushEventQueue( EventQueueRef queue )
{
	FlushEvents( everyEvent, 0 );
	
	return noErr;
}

// FindSpecificEventInQueue
// GetNumEventsInQueue
// RemoveEventFromQueue
// IsEventInQueue

pascal EventTime GetCurrentEventTime()
{
	UInt64 us;
	Microseconds( (UnsignedWide*) &us );
	
	return us / 1000000.0;
}

// IsUserCancelEventRef
// TrackMouseLocation
// TrackMouseLocationWithOptions
// TrackMouseRegion

pascal Boolean ConvertEventRefToEventRecord( EventRef event, EventRecord* outEvent )
{
	*outEvent = event->event;
	
	return true;
}

pascal Boolean IsEventInMask( EventRef event, EventMask mask )
{
	return (event->event.what & mask) != 0;
}

// GetLastUserEventTime

Boolean IsMouseCoalescingEnabled()
{
	return false;
}

// SetMouseCoalescingEnabled
// InstallEventLoopTimer
// RemoveEventLoopTimer
// SetEventLoopTimerNextFireTime
// CreateTypeStringWithOSType
// GetWindowEventTarget
// GetControlEventTarget
// GetMenuEventTarget
// GetApplicationEventTarget
// GetUserFocusEventTarget
// GetEventDispatcherTarget
// InstallEventHandler
// InstallStandardEventHandler
// RemoveEventHandler
// AddEventTypesToHandler
// RemoveEventTypesFromHandler
// CallNextEventHandler
// SendEventToEventTarget
// RegisterToolboxObjectClass
// UnregisterToolboxObjectClass
// ProcessHICommand
// RunApplicationEventLoop
// QuitApplicationEventLoop
// RunAppModalLoopForWindow
// QuitAppModalLoopForWindow
// BeginAppModalStateForWindow
// EndAppModalStateForWindow
// SetUserFocusWindow
// GetUserFocusWindow
// SetWindowDefaultButton
// SetWindowCancelButton
// GetWindowDefaultButton
// GetWindowCancelButton
// RegisterEventHotKey
// UnregisterEventHotKey

