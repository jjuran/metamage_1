/*
	Amaretto/Application.m
	----------------------
*/

#include "Amaretto/Application.h"


@implementation AmarettoApplication

- (void) sendEvent: (NSEvent*) event
{
	/*
		NSApplication's sendEvent method drops keyUp events
		when the Command key is down, so forward them here.
	*/
	
	if ( [event type] == NSKeyUp  &&  [event modifierFlags] & NSCommandKeyMask )
	{
		[[self keyWindow] sendEvent: event];
	}
	else
	{
		[super sendEvent: event];
	}
}

@end
