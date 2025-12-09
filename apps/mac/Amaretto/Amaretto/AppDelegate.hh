/*
	Amaretto/AppDelegate.hh
	-----------------------
*/

// Mac OS X
#import <Cocoa/Cocoa.h>


namespace raster
{
	
	struct clut_data;
	struct raster_desc;
	struct raster_load;
	
}

@interface AmarettoAppDelegate : NSObject
{
	void*                      _addr;
	const raster::raster_desc* _desc;
	const raster::clut_data*   _clut;
	
	long _imageSize;
	long _zoomLevel;
	
	id _mainWindow;
	id _mainGLView;
}

- (id) initWithRaster: (const raster::raster_load&) load;

- (void) dealloc;

- (void) doMenuItem: (id) sender;

- (void) applicationWillFinishLaunching: (NSNotification*) notification;

- (void) onRepaintDue;
- (void) onScreenBits;
- (void) onCursorBits;
- (void) onNewPalette;
- (void) onRasterDone;

@end
