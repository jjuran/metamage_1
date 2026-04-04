/*
	Amaretto/Application.h
	----------------------
*/

// Mac OS X
#import <Cocoa/Cocoa.h>


@interface AmarettoApplication : NSApplication
{
}

- (void) sendEvent: (NSEvent*) event;

@end
