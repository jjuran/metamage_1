/*
	Amaretto/AppDelegate.hh
	-----------------------
*/

// Mac OS X
#import <Cocoa/Cocoa.h>


namespace raster
{
	
	struct raster_desc;
	struct raster_load;
	
}

@interface AmarettoAppDelegate : NSObject
{
	const raster::raster_desc* _desc;
	
	long _zoomLevel;
	
	id _mainWindow;
	id _mainGLView;
}

- (id) initWithRaster: (const raster::raster_load&) load;

- (void) dealloc;

- (void) doMenuItem: (id) sender;

- (void) applicationWillFinishLaunching: (NSNotification*) notification;

@end
