/*
	WindowDelegate.mm
	-----------------
*/

#include "Amaretto/WindowDelegate.hh"


/*
	The window_moving flag is set in windowWillMove below.
	We can't unset it in windowDidMove, because that fires
	(repeatedly) prior to mouse-up.  Instead, it'll remain
	set until applicationDidBecomeActive checks and clears
	it.  Thus the name is oversimplified:  If the dragging
	click brings the application to the front, the flag is
	cleared immediately, while the window is still moving;
	otherwise, the flag persists until a layer switch that
	could happen arbitrarily far in the future (or perhaps
	never).  But only applicationDidBecomeActive reads it.
	
	Track the window bounds (content rect) last set by the
	user.  Our programmatic window resizing when the scale
	changes also invokes windowDidMove, so check a flag to
	avoid updating lastSetBounds incorrectly in that case.
*/

bool window_moving;
bool setting_scale;

NSRect lastSetBounds;


@implementation WindowDelegate

- (void) windowWillMove: (id) sender
{
	window_moving = true;
}

- (void) windowDidMove: (id) sender
{
	if ( ! setting_scale )
	{
		id window = [sender object];
		
		lastSetBounds = [window contentRectForFrameRect: [window frame]];
	}
}

@end
