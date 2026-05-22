/*
	Amaretto/FullscreenWindow.h
	---------------------------
*/

// Mac OS X
#import <Cocoa/Cocoa.h>


@interface FullscreenWindow : NSWindow
{
}

- (id) initWithWindow: (NSWindow*) window;

- (BOOL) canBecomeKeyWindow;

@end
