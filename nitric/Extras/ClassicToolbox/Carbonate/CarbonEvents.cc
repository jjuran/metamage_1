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

struct EventRefObject
{
	UInt32       retainCount;
	EventRecord  event;
	
	EventRefObject() : retainCount( 1 ), event( gNullEvent )  {}
	
	EventRefObject( const EventRefObject& other ) : retainCount( 1           ),
	                                                event      ( other.event )
	{
	}
	
	private:
		EventRefObject& operator=( const EventRefObject& other );
};

// These functions are always declared in the headers and are always extern.

pascal EventLoopRef GetCurrentEventLoop()
{
	return NULL;
}

pascal EventLoopRef GetMainEventLoop()
{
	return NULL;
}

pascal OSStatus RunCurrentEventLoop( EventTimeout timeout )
{
	EventRef ignored;
	
	return ReceiveNextEvent( 0, NULL, timeout, false, &ignored );
}

pascal OSStatus QuitEventLoop( EventLoopRef eventLoop )
{
	return paramErr;
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
		*outEvent = (EventRef) new EventRefObject();
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
		return (EventRef) new EventRefObject( *(EventRefObject*) other );
	}
	catch ( ... )
	{
		return NULL;
	}
}

pascal EventRef RetainEvent( EventRef event )
{
	++reinterpret_cast< EventRefObject* >( event )->retainCount;
	
	return event;
}

pascal UInt32 GetEventRetainCount( EventRef event )
{
	return reinterpret_cast< EventRefObject* >( event )->retainCount;
}

pascal void ReleaseEvent( EventRef event )
{
	EventRefObject* object = reinterpret_cast< EventRefObject* >( event );
	
	if ( --object->retainCount == 0 )
	{
		delete object;
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
	*outEvent = reinterpret_cast< EventRefObject* >( event )->event;
	
	return true;
}

pascal Boolean IsEventInMask( EventRef event, EventMask mask )
{
	return (reinterpret_cast< EventRefObject* >( event )->event.what & mask) != 0;
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

