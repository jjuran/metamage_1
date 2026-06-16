/*
	AppDelegate.mm
	--------------
*/

#include "Amaretto/AppDelegate.hh"

// POSIX
#include <unistd.h>

// mac-config
#include "mac_config/upp-macros.hh"

// mac-app-utils
#include "mac_app/quit.hh"

// write-a-splode
#include "splode/write-a-splode.hh"

// frontend-common
#include "frend/coprocess.hh"
#include "frend/cursor.hh"
#include "frend/zoom.hh"

// rasterlib
#include "raster/raster.hh"

// glfb-common
#include "glfb/glfb.hh"

// amicus
#include "amicus/apple_events.hh"
#include "amicus/cursor.hh"
#include "amicus/events.hh"
#include "amicus/menu_defs.hh"
#include "amicus/zoom.hh"

// Amaretto
#include "Amaretto/OpenGLView.h"
#include "releasing.hh"


using mac::app::quit;

using frend::cap_zoom_index;
using frend::coprocess_state;
using frend::current_zoom_index;
using frend::cursor_pinned;
using frend::launch_coprocess;
using frend::maximum_zoom_index;
using frend::wait_for_coprocess;
using frend::Zoom_index_2_0;

using amicus::command_ID_for_zoom_index;
using amicus::cursor_limit;
using amicus::events_fd;
using amicus::handle_Open_event;
using amicus::top_zoom_index;


static bool opened;

static coprocess_state coprocess;

int bindir_fd;

const char* works_path;

static
CGPoint window_contentRect_topLeft( NSWindow* window )
{
	NSRect outer = [[window screen] frame];
	NSRect inner = [window contentRectForFrameRect: [window frame]];
	
	CGFloat x = inner.origin.x;
	CGFloat y = outer.size.height - inner.origin.y - inner.size.height;
	
	return CGPointMake( x, y );
}

static
void synchronize_cursor_location( NSView* view )
{
	NSWindow* window = [view window];
	
	NSPoint v_origin = [view frame].origin;
	CGPoint w_origin = window_contentRect_topLeft( window );
	
	CGFloat scale = current_zoom_index / 2.0;
	
	CGFloat x = splode::last_sent_x * scale + v_origin.x + w_origin.x;
	CGFloat y = splode::last_sent_y * scale + v_origin.y + w_origin.y;
	
	CGWarpMouseCursorPosition( CGPointMake( x, y ) );
}

static
void update_cursor_location( AmarettoOpenGLView* view )
{
	if ( cursor_pinned )
	{
		return;
	}
	
	NSWindow* window = [view window];
	
	NSPoint origin = [window frame].origin;
	
	NSPoint pt = [NSEvent mouseLocation];
	
	pt.x -= origin.x;
	pt.y -= origin.y;
	
	[view handleMouseMovedTo: pt];
}

static
void launch_coprocess()
{
	coprocess = launch_coprocess( bindir_fd, works_path );
	
	events_fd = coprocess.socket_fd;
	
	close( bindir_fd );
}

static
pascal OSErr handle_Open_Apple_event( AppleEvent const* event,
                                      AppleEvent*       reply,
                                      SRefCon           refcon )
{
	OSErr err = handle_Open_event( event );
	
	if ( err == noErr )
	{
		if ( ! opened )
		{
			launch_coprocess();
			
			opened = true;
			
			AmarettoOpenGLView* view = (AmarettoOpenGLView*) refcon;
			
			update_cursor_location( view );
		}
	}
	
	return err;
}

DEFINE_UPP( AEEventHandler, handle_Open_Apple_event )

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
NSMenu* set_up_menus( unsigned default_zoom_command )
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
	
	return menu;  // View menu
}

@implementation AmarettoAppDelegate

- (id) initWithRaster: (const raster::raster_load&) load
{
	const raster::raster_desc& desc = load.meta->desc;
	
	NSSize space = [[NSScreen mainScreen] visibleFrame].size;
	
	cap_zoom_index( desc.width, desc.height, space.width, space.height );
	
	cursor_limit = CGPointMake( desc.width - 1, desc.height - 1 );
	
	_desc = &desc;
	
	_zoomLevel = kZoom100Percent;
	
	return self;
}

- (void) dealloc
{
	close( events_fd );
	
	if ( int pid = coprocess.child_pid )
	{
		wait_for_coprocess( pid );
	}
	
	if ( cursor_pinned )
	{
		synchronize_cursor_location( _mainGLView );
	}
	
	[_mainWindow release];
	
	glfb::terminate();
	
	[super dealloc];
}

- (void) setCursorPinning: (BOOL) pinning
{
	using amicus::ignore_next_mouse_moved_event;
	using amicus::set_cursor_hidden;
	
	/*
		If we're pinning a visible cursor, hide it.
		(We don't show the cursor when unpinning it,
		because it stays within the window.)
	*/
	
	cursor_pinned = pinning;
	
	if ( pinning )
	{
		ignore_next_mouse_moved_event = true;
		
		set_cursor_hidden( true );
	}
	
	synchronize_cursor_location( _mainGLView );
	
	CGAssociateMouseAndMouseCursorPosition( ! pinning );
}

- (void) doZoom: (long) commandID
{
	using frend::current_zoom_index;
	
	using amicus::cursor_hidden;
	
	const long tag = commandID;
	
	if ( tag != _zoomLevel )
	{
		CGFloat x = current_zoom_index / 2.0;
		
		[_mainGLView setScale: x];
		
		[[_viewMenu itemWithTag: _zoomLevel ] setState: NSOffState];
		[[_viewMenu itemWithTag: commandID  ] setState: NSOnState ];
		
		_zoomLevel = tag;
		
		if ( cursor_hidden )
		{
			[self setCursorPinning: cursor_pinned];
		}
	}
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
				using frend::current_zoom_index;
				
				int  _50 = tag >> 16 & 0x1;
				int _100 = tag >> 24 & 0xf;
				
				int x_denom =  _50 + 1;              // 1 or 2
				int x_numer = _100 * x_denom + _50;
				
				current_zoom_index = 2 * x_numer / x_denom;
				
				[self doZoom: tag];
			}
			break;
		
		default:
			break;
	}
}

- (void) applicationWillFinishLaunching: (NSNotification*) notification
{
	OSStatus err;
	
	const int x2 = Zoom_index_2_0;  // 200%
	
	current_zoom_index = maximum_zoom_index < x2 ? maximum_zoom_index : x2;
	
	_zoomLevel = command_ID_for_zoom_index( current_zoom_index );
	
	err = AEInstallEventHandler( kCoreEventClass,
	                             kAEQuitApplication,
	                             UPP_ARG( handle_Quit_Apple_event ),
	                             0,
	                             false );
	
	_viewMenu = set_up_menus( _zoomLevel );
	
	_mainWindow = create_window( *_desc, current_zoom_index / 2.0 );
	_mainGLView = [_mainWindow initialFirstResponder];
	
	/*
		Installing a handler for event ID typeWildCard doesn't work;
		we need kAEOpenApplication and kAEOpenDocuments explicitly.
	*/
	
	err = AEInstallEventHandler( kCoreEventClass,
	                             kAEOpenApplication,
	                             UPP_ARG( handle_Open_Apple_event ),
	                             (SRefCon) _mainGLView,
	                             false );
	
	err = AEInstallEventHandler( kCoreEventClass,
	                             kAEOpenDocuments,
	                             UPP_ARG( handle_Open_Apple_event ),
	                             (SRefCon) _mainGLView,
	                             false );
}

- (void) applicationDidBecomeActive: (NSNotification*) notification
{
	if ( events_fd >= 0 )
	{
		update_cursor_location( _mainGLView );
	}
}

@end
