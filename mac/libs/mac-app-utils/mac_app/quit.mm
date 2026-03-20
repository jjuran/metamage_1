/*
	quit.mm
	-------
*/

#include "mac_app/quit.hh"

// Mac OS X
#import <Cocoa/Cocoa.h>


namespace mac {
namespace app {

void quit_Cocoa()
{
	[NSApp stop: nil];
	
	NSEvent* dummy = [NSEvent otherEventWithType: NSApplicationDefined
	                          location:           NSZeroPoint
	                          modifierFlags:      0
	                          timestamp:          0
	                          windowNumber:       0
	                          context:            nil
	                          subtype:            0
	                          data1:              0
	                          data2:              0];
	
	[NSApp postEvent: dummy atStart: YES];
}

}
}
