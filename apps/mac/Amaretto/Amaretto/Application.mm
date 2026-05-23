/*
	Amaretto/Application.mm
	-----------------------
*/

#include "Amaretto/Application.h"


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
	else
	{
		[super sendEvent: event];
	}
}

@end
