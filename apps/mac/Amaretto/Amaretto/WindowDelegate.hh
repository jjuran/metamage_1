/*
	Amaretto/WindowDelegate.hh
	--------------------------
*/

// Mac OS X
#import <Cocoa/Cocoa.h>


#ifdef MAC_OS_X_VERSION_10_6
#define WINDOWDELEGATE_PROTOCOLS  < NSWindowDelegate >
#else
#define WINDOWDELEGATE_PROTOCOLS  /**/
#endif


extern bool window_moving;


@interface WindowDelegate : NSObject WINDOWDELEGATE_PROTOCOLS
{
}

- (void) windowWillMove: (id) sender;

@end
