/*
	Amaretto/FullscreenWindow.m
	---------------------------
	
	Inspiration:
	
	https://www.cocoawithlove.com/2009/08/animating-window-to-fullscreen-on-mac.html
*/

#include "Amaretto/FullscreenWindow.h"


@implementation FullscreenWindow

- (id) initWithWindow: (NSWindow*) window
{
	NSScreen* screen = [window screen];
	
	[super initWithContentRect: [screen frame]
	       styleMask:           NSBorderlessWindowMask
	       backing:             NSBackingStoreBuffered
	       defer:               YES];
	
	[self setLevel: NSFloatingWindowLevel];
	[self setTitle: [window title      ] ];
	
	return self;
}

- (BOOL) canBecomeKeyWindow
{
	/*
		NSWindow's implementation of this method returns NO for
		NSBorderlessWindowMask windows, so we need to override it.
	*/
	
	return YES;
}

@end
