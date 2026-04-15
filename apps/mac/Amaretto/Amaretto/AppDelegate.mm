/*
	AppDelegate.mm
	--------------
*/

#include "Amaretto/AppDelegate.hh"

// mac-config
#include "mac_config/upp-macros.hh"

// mac-app-utils
#include "mac_app/quit.hh"

// frontend-common
#include "frend/zoom.hh"

// rasterlib
#include "raster/raster.hh"

// glfb-common
#include "glfb/glfb.hh"

// amicus
#include "amicus/menu_defs.hh"
#include "amicus/zoom.hh"

// Amaretto
#include "Amaretto/OpenGLView.h"
#include "releasing.hh"


using mac::app::quit;

using frend::cap_zoom_index;
using frend::current_zoom_index;
using frend::maximum_zoom_index;

using amicus::command_ID_for_zoom_index;
using amicus::top_zoom_index;


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
NSWindow* create_window( const raster::raster_desc& desc, CGFloat scale )
{
	int width  = desc.width;
	int height = desc.height;
	
	NSRect contentRect = NSMakeRect( 0, 0, width * scale, height * scale );
	
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
	
	[view setScale: scale];
	
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
void set_up_menus( unsigned default_zoom_command )
{
	id menubar = [NSMenu new];
	
	[NSApp setMainMenu: releasing( menubar )];
	
	NSMenu* menu;
	
	menu = SET_UP_MENU( Apple );
	menu = SET_UP_MENU( File  );
	menu = SET_UP_MENU( Edit  );
	menu = SET_UP_MENU( View  );
	
	[[menu itemWithTag: default_zoom_command] setState: NSOnState];
	
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
	
	_desc = &desc;
	
	_zoomLevel = kZoom100Percent;
	
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
	
	const int x2 = 0x200 >> 7;  // 4
	
	current_zoom_index = maximum_zoom_index < x2 ? maximum_zoom_index : x2;
	
	_zoomLevel = command_ID_for_zoom_index( current_zoom_index );
	
	err = AEInstallEventHandler( kCoreEventClass,
	                             kAEQuitApplication,
	                             UPP_ARG( handle_Quit_Apple_event ),
	                             0,
	                             false );
	
	set_up_menus( _zoomLevel );
	
	_mainWindow = create_window( *_desc, current_zoom_index / 2.0 );
	_mainGLView = [_mainWindow initialFirstResponder];
	
	NSRect frame = [_mainWindow frame];
	
	NSPoint pt = [NSEvent mouseLocation];
	
	pt.x -= frame.origin.x;
	pt.y -= frame.origin.y;
	
	[_mainGLView handleMouseMovedTo: pt];
}

@end
