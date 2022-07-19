/*
	application.cc
	--------------
*/

#include "aglow/application.hh"

// Mac OS X
#ifdef __APPLE__
#include <Carbon/Carbon.h>
#endif

// POSIX
#include <unistd.h>

// missing-macos
#ifdef MAC_OS_X_VERSION_10_7
#ifndef MISSING_QUICKDRAW_H
#include "missing/Quickdraw.h"
#endif
#endif

// mac-config
#include "mac_config/upp-macros.hh"

// mac-app-utils
#include "mac_app/menus.hh"

// splode
#include "splode/splode.hh"

// rasterlib
#include "raster/raster.hh"

// amicus
#include "amicus/events.hh"
#include "amicus/keycodes.hh"
#include "amicus/raster_task.hh"
#include "amicus/splode.hh"

// aglow
#include "aglow/blit_AGL.hh"
#include "aglow/menus.hh"
#include "aglow/window.hh"


#define LENGTH( array )  (sizeof (array) / sizeof *(array))


static WindowRef screen_window;

static MenuCommand current_zoom = kZoom100Percent;

static bool mid_click;


namespace amicus
{

using raster::raster_desc;
using raster::raster_load;

static bool cursor_hidden;

static
void blit( const raster_load& load )
{
	const raster_desc& desc = load.meta->desc;
	
	const uint32_t offset = desc.height * desc.stride * desc.frame;
	
	blit_AGL( (Ptr) load.addr + offset );
}

static
void remake_window( const raster_load& load )
{
	WindowRef window = create_window( load.meta->desc, screen_window );
	
	attach_to_window( window );
	
	blit( load );
	
	ShowWindow( window );
	
	DisposeWindow( screen_window );
	
	screen_window = window;
}

static
pascal OSStatus AmicusUpdate( EventHandlerCallRef  handler,
                              EventRef             event,
                              void*                userData )
{
	const raster_load& load = *(raster_load*) userData;
	
	blit( load );
	
	return noErr;
}

DEFINE_CARBON_UPP( EventHandler, AmicusUpdate )

static EventTypeSpec AmicusUpdate_event[] =
{
	{ kEventClassAmicus, kEventAmicusUpdate },
};

static
pascal OSStatus ProcessCommand( EventHandlerCallRef  handler,
                                EventRef             event,
                                void*                userData )
{
	OSStatus err;
	
	const raster_load& load = *(raster_load*) userData;
	
	HICommand command = {};
	err = GetEventParameter( event,
	                         kEventParamDirectObject,
	                         typeHICommand,
	                         NULL,
	                         sizeof command,
	                         NULL,
	                         &command );
	
	const MenuCommand id = command.commandID;
	
	if ( id == current_zoom )
	{
		return noErr;
	}
	
	switch ( id )
	{
		case kZoom50Percent:   // 1/2
		case kZoom100Percent:  // 1/1
		case kZoom150Percent:  // 3/2
		case kZoom200Percent:  // 2/1
		case kZoom300Percent:  // 3/1
			x_denom = 1 + (id >> 16 & 0x1);
			x_numer = ((id >> 24 & 0xf) + 1) * x_denom - 1;
			
			SetMenuCommandMark( View, current_zoom, noMark );
			SetMenuCommandMark( View, id, kCheckCharCode );
			
			current_zoom = id;
			
			remake_window( load );
			
			return noErr;
		
		default:
			break;
	}
	
	return eventNotHandledErr;
}

DEFINE_CARBON_UPP( EventHandler, ProcessCommand )

static EventTypeSpec ProcessCommand_event[] =
{
	{ kEventClassCommand, kEventCommandProcess },
};

static
pascal OSStatus Mouse_action( EventHandlerCallRef  handler,
                              EventRef             event,
                              void*                userData )
{
	using namespace splode::modes;
	using namespace splode::key;
	
	OSStatus err;
	
	if ( FrontWindow() != screen_window )
	{
		return eventNotHandledErr;
	}
	
	EventKind kind = GetEventKind( event );
	
	HIPoint location = {};
	err = GetEventParameter( event,
	                         kEventParamMouseLocation,
	                         typeHIPoint,
	                         NULL,
	                         sizeof location,
	                         NULL,
	                         &location );
	
	UInt32 modifiers = 0;
	err = GetEventParameter( event,
	                         kEventParamKeyModifiers,
	                         typeUInt32,
	                         NULL,
	                         sizeof (UInt32),
	                         NULL,
	                         &modifiers );
	
	Rect bounds;
	GetWindowBounds( screen_window, kWindowGlobalPortRgn, &bounds );
	
	const uint8_t mode_mask = Command | Shift | Option | Control;
	
	const uint8_t modes = (modifiers >> 8) & mode_mask;
	
	Point pt = { (short) location.y, (short) location.x };
	
	if ( PtInRect( pt, &bounds ) )
	{
		if ( ! cursor_hidden )
		{
			cursor_hidden = true;
			
			CGDisplayHideCursor( 0 );
		}
		
		short x = pt.h - bounds.left;
		short y = pt.v - bounds.top;
		
		x *= x_denom;
		y *= x_denom;
		
		x /= x_numer;
		y /= x_numer;
		
		send_mouse_moved_event( events_fd, x, y );
		
		if ( kind <= 2 )
		{
			mid_click = kind == 1;
			
			send_mouse_event( events_fd, kind, modes );
			return noErr;
		}
	}
	else if ( cursor_hidden )
	{
		cursor_hidden = false;
		
		CGDisplayShowCursor( 0 );
	}
	
	if ( mid_click  &&  kind == 2 )
	{
		mid_click = false;
		
		send_mouse_event( events_fd, kind, modes );
		return noErr;
	}
	
	return eventNotHandledErr;
}

DEFINE_CARBON_UPP( EventHandler, Mouse_action )

static EventTypeSpec Mouse_event[] =
{
	{ kEventClassMouse, kEventMouseDown    },
	{ kEventClassMouse, kEventMouseUp      },
	{ kEventClassMouse, kEventMouseMoved   },
	{ kEventClassMouse, kEventMouseDragged },
};

static
pascal OSStatus Keyboard_action( EventHandlerCallRef  handler,
                                 EventRef             event,
                                 void*                userData )
{
	using namespace splode::modes;
	using namespace splode::key;
	
	OSStatus err;
	
	if ( FrontWindow() != screen_window )
	{
		return eventNotHandledErr;
	}
	
	UInt32 vcode = 0;
	err = GetEventParameter( event,
	                         kEventParamKeyCode,
	                         typeUInt32,
	                         NULL,
	                         sizeof (UInt32),
	                         NULL,
	                         &vcode );
	
	UInt32 modifiers = 0;
	err = GetEventParameter( event,
	                         kEventParamKeyModifiers,
	                         typeUInt32,
	                         NULL,
	                         sizeof (UInt32),
	                         NULL,
	                         &modifiers );
	
	const signed char c = lookup_from_virtual[ vcode & 0x7f ];
	
	const uint8_t mode_mask = Command | Shift | Option | Control;
	const uint8_t attr_mask = Alpha;
	
	EventKind kind = GetEventKind( event );
	
	const uint8_t action = kind ^ (kind > 1);  // 1, 2, 3 -> 1, 3, 2
	const uint8_t keypad = is_keypad( vcode ) ? Keypad : 0;
	
	const uint8_t modes =  (modifiers >> 8) & mode_mask;
	const uint8_t attrs = ((modifiers >> 8) & attr_mask) | keypad | action;
	
	send_key_event( events_fd, c, modes, attrs );
	
	return noErr;
}

DEFINE_CARBON_UPP( EventHandler, Keyboard_action )

static EventTypeSpec Keyboard_event[] =
{
	{ kEventClassKeyboard, kEventRawKeyDown   },
	{ kEventClassKeyboard, kEventRawKeyRepeat },
	{ kEventClassKeyboard, kEventRawKeyUp     },
};

void run_event_loop( const raster_load& load, const raster_desc& desc )
{
	OSStatus err;
	
	/*
		We have to launch the coprocess before we create the window in
		Mac OS X 10.4, or the application will immediately terminate after
		displaying the window when launched from the Finder.  (It runs
		normally when Amber is launched explicitly, via command line.)
		
		This appears to be related to a similar failure observed a year
		earlier, in which the window was created by BeginFullScreen() (in
		QuickTime) rather than CreateNewWindow().  in that case, creating
		the window before launching the coprocess resulted in a corrupted
		socket, calling write() on which killed the process.
	*/
	
	create_AGL_context();
	
	set_AGL_geometry( desc.stride, desc.width, desc.height );
	
	screen_window = create_window( desc, NULL );
	
	attach_to_window( screen_window );
	
	blit( load );
	
	ShowWindow( screen_window );
	
	EventHandlerRef updateHandler;
	err = InstallApplicationEventHandler( UPP_ARG( AmicusUpdate ),
	                                      LENGTH( AmicusUpdate_event ),
	                                      AmicusUpdate_event,
	                                      (void*) &load,
	                                      &updateHandler );
	
	EventHandlerRef commandHandler;
	err = InstallApplicationEventHandler( UPP_ARG( ProcessCommand ),
	                                      LENGTH( ProcessCommand_event ),
	                                      ProcessCommand_event,
	                                      (void*) &load,
	                                      &commandHandler );
	
	EventHandlerRef mouseHandler;
	err = InstallApplicationEventHandler( UPP_ARG( Mouse_action ),
	                                      LENGTH( Mouse_event ),
	                                      Mouse_event,
	                                      NULL,
	                                      &mouseHandler );
	
	EventHandlerRef keyboardHandler;
	err = InstallApplicationEventHandler( UPP_ARG( Keyboard_action ),
	                                      LENGTH( Keyboard_event ),
	                                      Keyboard_event,
	                                      NULL,
	                                      &keyboardHandler );
	
	mac::app::set_Aqua_menu_key( kHICommandQuit, '\0' );
	mac::app::set_Aqua_menu_key( kHICommandHide, '\0' );
	
	const Fixed max_zoom = maximum_zoom( desc.width, desc.height );
	
	do
	{
		if ( max_zoom >= 0x30000 )  break;
		DisableMenuCommand( View, kZoom300Percent );
		
		if ( max_zoom >= 0x20000 )  break;
		DisableMenuCommand( View, kZoom200Percent );
		
		if ( max_zoom >= 0x18000 )  break;
		DisableMenuCommand( View, kZoom150Percent );
	}
	while ( false );
	
	/*
		Install a Carbon Event handler to respond to
		changes to the currently active modifier keys.
	*/
	
	amicus::install_rawKey_handler();
	
	RunApplicationEventLoop();
	
	close( events_fd );
	
	destroy_AGL_context();
	
	DisposeWindow( screen_window );
}

void initialize()
{
	set_up_menus();
	
	SetMenuCommandMark( View, kZoom100Percent, kCheckCharCode );
}

}  // namespace amicus
