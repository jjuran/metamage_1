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

extern int bindir_fd;

extern const char* works_path;

@interface AmarettoAppDelegate : NSObject
{
	const raster::raster_desc* _desc;
	
	long _zoomLevel;
	
	id _viewMenu;
	id _mainWindow;
	id _mainGLView;
}

- (id) initWithRaster: (const raster::raster_load&) load;

- (void) destruct;

- (void) setCursorPinning: (BOOL) pinning;

- (void) doZoom: (long) commandID;

- (void) doMenuItem: (id) sender;

- (void) applicationWillFinishLaunching: (NSNotification*) notification;

- (void) applicationDidBecomeActive: (NSNotification*) notification;

@end
