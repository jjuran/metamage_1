/*
	Amaretto/Application.mm
	-----------------------
*/

#include "Amaretto/Application.h"

// frontend-common
#include "frend/cursor_invisible.hh"

// amicus
#include "amicus/events.hh"

// Amaretto
#include "Amaretto/AppDelegate.hh"


using frend::cursor_invisible;


static
BOOL is_mouse_event( NSEventType type )
{
	switch ( type )
	{
		case NSMouseMoved:
		case NSLeftMouseDragged:
		case NSRightMouseDragged:
			return true;
		
		default:
			return false;
	}
}

@implementation AmarettoApplication

- (void) sendEvent: (NSEvent*) event
{
	NSEventType type = [event type];
	
	/*
		NSApplication's sendEvent method drops keyUp events
		when the Command key is down, so forward them here.
	*/
	
	if ( type == NSKeyUp  &&  [event modifierFlags] & NSCommandKeyMask )
	{
		[[self keyWindow] sendEvent: event];
	}
	else if ( type == NSLeftMouseUp  &&  pin_postponed )
	{
		/*
			If we postponed pinning the cursor (on resume)
			because the window was being moved, do it here.
		*/
		
		pin_postponed = false;
		
		AmarettoAppDelegate* delegate = (AmarettoAppDelegate*) [NSApp delegate];
		
		[delegate setCursorPinning: cursor_invisible()];
	}
	else if ( frend::cursor_pinned  &&  is_mouse_event( type ) )
	{
		amicus::handle_CGEvent( [event CGEvent] );
	}
	else
	{
		[super sendEvent: event];
	}
}

@end
