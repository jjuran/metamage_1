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
*/

bool window_moving;


@implementation WindowDelegate

- (void) windowWillMove: (id) sender
{
	window_moving = true;
}

@end
