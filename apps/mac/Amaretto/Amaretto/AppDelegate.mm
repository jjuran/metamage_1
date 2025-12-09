/*
	AppDelegate.mm
	--------------
*/

#include "Amaretto/AppDelegate.hh"

// v68k-cursor
#include "cursor/cursor.hh"

// mac-config
#include "mac_config/upp-macros.hh"

// rasterlib
#include "raster/clut.hh"
#include "raster/clut_detail.hh"
#include "raster/raster.hh"

// glfb-common
#include "glfb/glfb.hh"

// frontend-common
#include "frend/cursor.hh"

// amicus
#include "amicus/menu_defs.hh"
#include "amicus/zoom.hh"

// Amaretto
#include "Amaretto/OpenGLView.h"
#include "releasing.hh"


using amicus::cap_zoom_index;
using amicus::command_ID_for_zoom_index;
using amicus::maximum_zoom_index;
using amicus::top_zoom_index;


static
void quit()
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

static
pascal OSErr handle_Quit_Apple_event( AppleEvent const* event,
                                      AppleEvent*       reply,
                                      SRefCon           refcon )
{
	quit();
	
	return noErr;
}

DEFINE_UPP( AEEventHandler, handle_Quit_Apple_event )

static
NSWindow* create_window( const raster::raster_desc& desc )
{
	int width  = desc.width;
	int height = desc.height;
	
	NSRect contentRect = NSMakeRect( 0, 0, width, height );
	
	id window = [[NSWindow alloc] initWithContentRect: contentRect
	                              styleMask:           NSTitledWindowMask
	                              backing:             NSBackingStoreBuffered
	                              defer:               NO];
	
	id viewContainer = window;
	
	if ( kScreenMargin > 0 )
	{
		id black = [NSColor blackColor];
		id box   = [NSBox new];
		
		[box    setBoxType:         NSBoxCustom];
		[box    setFillColor:       black];
		[window setBackgroundColor: black];
		[window setContentView:     releasing( box )];
		
		viewContainer = box;
	}
	
	CGSize resolution = CGSizeMake( width, height );
	
	id view = [[AmarettoOpenGLView alloc] initWithResolution: resolution];
	
	[viewContainer setContentView: releasing( view )];
	
	id appName = [[NSProcessInfo processInfo] processName];
	
	[(NSWindow*) window center];
	
	[window setTitle:                   appName];
	[window makeKeyAndOrderFront:       nil];
	[window setAcceptsMouseMovedEvents: YES];
	[window setInitialFirstResponder:   view];
	
	[view setScale: 1.0];
	
	glfb::initialize();
	
	glfb::set_dimensions( desc.width, desc.height, desc.weight );
	
	return window;
}

static
NSMenu* set_up_menu( NSMenu* menubar, const menu_description& desc )
{
	NSMenu* menu = [NSMenu new];
	
	[menu setTitle: desc.title ];
	
	for ( short i = 0;  i < desc.count; )
	{
		const menu_item_description& item = desc.items[ i++ ];
		
		id menuItem = [[NSMenuItem separatorItem] retain];
		
		if ( item.text )
		{
			[menuItem release];
			
			menuItem = [NSMenuItem new];
			
			[menuItem setTitle: item.text];
			
			[menuItem setAction: @selector(doMenuItem:)];
			
			if ( item.command )
			{
				[menuItem setTag: item.command];
			}
		}
		
		[menu addItem: releasing( menuItem )];
	}
	
	id menuItem = [NSMenuItem new];
	
	[menuItem setSubmenu: releasing( menu )];
	
	[menubar addItem: releasing( menuItem )];
	
	return menu;
}

#define SET_UP_MENU( name )  set_up_menu( menubar, name##_desc )

static
void set_up_menus()
{
	id menubar = [NSMenu new];
	
	[NSApp setMainMenu: releasing( menubar )];
	
	NSMenu* menu;
	
	menu = SET_UP_MENU( Apple );
	menu = SET_UP_MENU( File  );
	menu = SET_UP_MENU( Edit  );
	menu = SET_UP_MENU( View  );
	
	[[menu itemWithTag: kZoom100Percent] setState: NSOnState];
	
	[menu setAutoenablesItems: NO];
	
	for ( int i = top_zoom_index;  i > maximum_zoom_index;  --i )
	{
		MenuCommand command_ID = command_ID_for_zoom_index( i );
		
		[[menu itemWithTag: command_ID] setEnabled: NO];
	}
}

@implementation AmarettoAppDelegate

- (id) initWithRaster: (const raster::raster_load&) load
{
	const raster::raster_desc& desc = load.meta->desc;
	
	NSSize space = [[NSScreen mainScreen] frame].size;
	
	cap_zoom_index( desc.width, desc.height, space.width, space.height );
	
	_addr = load.addr;
	_desc = &desc;
	_clut = find_clut( &load.meta->note );
	
	_imageSize = desc.stride * desc.height;
	_zoomLevel = kZoom100Percent;
	
	glfb::cursor_enabled = frend::cursor_state != NULL;
	
	return self;
}

- (void) dealloc
{
	[_mainWindow release];
	
	glfb::terminate();
	
	[super dealloc];
}

- (void) doMenuItem: (id) sender
{
	long tag = [sender tag];
	
	switch ( tag )
	{
		case kHICommandQuit:
			quit();
			break;
		
		case kHICommandAbout:
			[NSApp orderFrontStandardAboutPanel: sender];
			break;
		
		case kHICommandClose:
			if ( NSWindow* window = [NSApp keyWindow] )
			{
				[window close];
			}
			break;
		
		case kZoom50Percent:
		case kZoom100Percent:
		case kZoom150Percent:
		case kZoom200Percent:
		case kZoom250Percent:
		case kZoom300Percent:
		case kZoom350Percent:
		case kZoom400Percent:
			if ( tag != _zoomLevel )
			{
				int  _50 = tag >> 16 & 0x1;
				int _100 = tag >> 24 & 0xf;
				
				int x_denom =  _50 + 1;              // 1 or 2
				int x_numer = _100 * x_denom + _50;
				
				CGFloat x = 1.0 * x_numer / x_denom;
				
				[_mainGLView setScale: x];
				
				[[[sender menu] itemWithTag: _zoomLevel ] setState: NSOffState];
				
				[sender setState: NSOnState];
				
				_zoomLevel = tag;
			}
			break;
		
		default:
			break;
	}
}

- (void) applicationWillFinishLaunching: (NSNotification*) notification
{
	OSStatus err;
	
	err = AEInstallEventHandler( kCoreEventClass,
	                             kAEQuitApplication,
	                             UPP_ARG( handle_Quit_Apple_event ),
	                             0,
	                             false );
	
	set_up_menus();
	
	_mainWindow = create_window( *_desc );
	_mainGLView = [_mainWindow initialFirstResponder];
	
	[self onCursorBits];
}

- (void) onRepaintDue
{
	using frend::cursor_state;
	using frend::shared_cursor_state;
	
	if ( const shared_cursor_state* cursor = cursor_state )
	{
		int y  = cursor->pointer[ 0 ];
		int x  = cursor->pointer[ 1 ];
		int dy = cursor->hotspot[ 0 ];
		int dx = cursor->hotspot[ 1 ];
		
		glfb::set_cursor_hotspot( dx, dy );
		glfb::set_cursor_location( x, y );
		glfb::set_cursor_visibility( cursor->visible );
	}
	
	[_mainGLView setNeedsDisplay: YES];
}

- (void) onScreenBits
{
	const uint32_t offset = _imageSize * _desc->frame;
	
	glfb::set_screen_image( (Ptr) _addr + offset );
}

- (void) onCursorBits
{
	using frend::cursor_state;
	using frend::shared_cursor_state;
	
	if ( const shared_cursor_state* cursor = cursor_state )
	{
		glfb::set_cursor_image( cursor );
	}
}

- (void) onNewPalette
{
	if ( _clut )
	{
		glfb::set_palette( &_clut->palette[ 0 ].value, _clut->max + 1 );
	}
}

- (void) onRasterDone
{
	quit();
}

@end
