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

extern bool pin_postponed;

extern int bindir_fd;

extern const char* works_path;

void update_scale( unsigned image_width, unsigned image_height );

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

- (void) setCursorEjected: (BOOL) ejected;
- (void) setCursorPinning: (BOOL) pinning;

- (void) doZoom: (long) commandID;

- (void) doMenuItem: (id) sender;

- (BOOL) validateMenuItem: (NSMenuItem*) menuItem;

- (void) applicationWillFinishLaunching: (NSNotification*) notification;

- (void) applicationDidBecomeActive: (NSNotification*) notification;
- (void) applicationDidResignActive: (NSNotification*) notification;

@end
