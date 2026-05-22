/*
	Amaretto/Application.mm
	-----------------------
*/

#include "Amaretto/Application.h"

// frontend-common
#include "frend/cursor.hh"

// amicus
#include "amicus/events.hh"


static
BOOL is_mouse_event( NSEventType type )
{
	switch ( type )
	{
		case NSLeftMouseDown:
		case NSLeftMouseUp:
		case NSRightMouseDown:
		case NSRightMouseUp:
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
