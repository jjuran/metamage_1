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

// v68k-cursor
#include "cursor/cursor.hh"

// mac-app-utils
#include "mac_app/menus.hh"

// splode
#include "splode/splode.hh"

// rasterlib
#include "raster/raster.hh"

// glfb-common
#include "glfb/glfb.hh"

// amicus
#include "amicus/cursor.hh"
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

using glfb::overlay_enabled;

using raster::raster_desc;
using raster::raster_load;

using v68k::cursor::shared_cursor_state;

static bool cursor_hidden;

static bool Q_hit;
static bool X_hit;

static
void clear_chords()
{
	Q_hit = false;
	X_hit = false;
}

static
void blit( const raster_load& load )
{
	const raster_desc& desc = load.meta->desc;
	
	const uint32_t offset = desc.height * desc.stride * desc.frame;
	
	glfb::set_screen_image( (Ptr) load.addr + offset );
	
	render_AGL();
}

static
void remake_window( const raster_load& load )
{
	WindowRef window = create_window( load.meta->desc, screen_window );
	
	attach_to_window( window );
	
	render_AGL();
	
	ShowWindow( window );
	
	DisposeWindow( screen_window );
	
	screen_window = window;
}

static
pascal OSStatus AmicusUpdate( EventHandlerCallRef  handler,
                              EventRef             event,
                              void*                userData )
{
	EventKind kind = GetEventKind( event );
	
	if ( kind == kEventAmicusScreenBits )
	{
		const raster_load& load = *(raster_load*) userData;
		const raster_desc& desc = load.meta->desc;
		
		const uint32_t offset = desc.height * desc.stride * desc.frame;
		
		glfb::set_screen_image( (Ptr) load.addr + offset );
		
		return noErr;
	}
	
	if ( const amicus::shared_cursor_state* cursor = amicus::cursor_state )
	{
		if ( kind == kEventAmicusCursorBits )
		{
			glfb::set_cursor_image( cursor );
			
			return noErr;
		}
		
		int y  = cursor->pointer[ 0 ];
		int x  = cursor->pointer[ 1 ];
		int dy = cursor->hotspot[ 0 ];
		int dx = cursor->hotspot[ 1 ];
		
		glfb::set_cursor_hotspot( dx, dy );
		glfb::set_cursor_location( x, y );
		glfb::set_cursor_visibility( cursor->visible );
	}
	
	render_AGL();
	
	return noErr;
}

DEFINE_CARBON_UPP( EventHandler, AmicusUpdate )

static EventTypeSpec AmicusUpdate_event[] =
{
	{ kEventClassAmicus, kEventAmicusUpdate },
	{ kEventClassAmicus, kEventAmicusScreenBits },
	{ kEventClassAmicus, kEventAmicusCursorBits },
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
		case kZoom250Percent:  // 5/2
		case kZoom300Percent:  // 3/1
		case kZoom350Percent:  // 7/2
		case kZoom400Percent:  // 4/1
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

static inline
bool any_keys_down()
{
	KeyMap  keymap;
	UInt32* keys = (UInt32*) keymap;
	
	GetKeys( keymap );
	
	/*
		These will be endian-swapped on x86 and ARM,
		but we're only checking for zero so we don't care.
	*/
	
	return keys[ 0 ]  ||  keys[ 1 ]  ||  keys[ 2 ]  ||  keys[ 3 ];
}

static
void command_handler( char c )
{
	switch ( c )
	{
		case 'q':  Q_hit = true;  break;
		case 'x':  X_hit = true;  break;
		
		default:
			break;
	}
	
	if ( Q_hit  &&  X_hit )
	{
		QuitApplicationEventLoop();
	}
}

static
pascal OSStatus Keyboard_action( EventHandlerCallRef  handler,
                                 EventRef             event,
                                 void*                userData )
{
	using namespace splode::key;
	
	OSStatus err;
	
	const raster_load& load = *(raster_load*) userData;
	
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
	
	const signed char c = lookup_from_virtual[ vcode & 0x7f ];
	
	EventKind kind = GetEventKind( event );
	
	const uint8_t action = kind ^ (kind > 1);  // 1, 2, 3 -> 1, 3, 2
	const uint8_t keypad = is_keypad( vcode ) ? Keypad : 0;
	
	if ( commandmode_state )
	{
		switch ( kind )
		{
			case kEventRawKeyDown:
				command_handler( c );
				break;
			
			case kEventRawKeyUp:
				switch ( commandmode_state )
				{
					case CommandMode_activated:
						commandmode_state = CommandMode_quasimode;
						break;
					
					case CommandMode_oneshot:
						if ( ! any_keys_down() )
						{
							commandmode_state = CommandMode_off;
							overlay_enabled   = false;
							
							render_AGL();
						}
						break;
					
					default:
						break;
				}
				break;
		
			default:
				break;
		}
		
		return noErr;
	}
	
	CommandMode_state prev_state = commandmode_state;
	
	err = amicus::send_key_event( event, c, keypad | action );
	
	if ( ! commandmode_state != ! prev_state )
	{
		overlay_enabled = ! overlay_enabled;
		
		render_AGL();
	}
	
	return err;
}

DEFINE_CARBON_UPP( EventHandler, Keyboard_action )

static EventTypeSpec Keyboard_event[] =
{
	{ kEventClassKeyboard, kEventRawKeyDown   },
	{ kEventClassKeyboard, kEventRawKeyRepeat },
	{ kEventClassKeyboard, kEventRawKeyUp     },
};

static
pascal OSStatus Modifiers_changed( EventHandlerCallRef  handler,
                                   EventRef             event,
                                   void*                userData )
{
	OSStatus err;
	
	const raster_load& load = *(raster_load*) userData;
	
	CommandMode_state prev_state = commandmode_state;
	
	err = amicus::send_key_event( event, '\0' );
	
	if ( ! commandmode_state )
	{
		clear_chords();
	}
	
	if ( ! commandmode_state != ! prev_state )
	{
		overlay_enabled = ! overlay_enabled;
		
		render_AGL();
	}
	
	return err;
}

DEFINE_CARBON_UPP( EventHandler, Modifiers_changed )

static EventTypeSpec Modifiers_event[] =
{
	{ kEventClassKeyboard, kEventRawKeyModifiersChanged },
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
	
	glfb::cursor_enabled = amicus::cursor_state != NULL;
	
	create_AGL_context();
	
	set_AGL_geometry( desc.stride, desc.width, desc.height, desc.weight );
	
	screen_window = create_window( desc, NULL );
	
	attach_to_window( screen_window );
	
	if ( const amicus::shared_cursor_state* cursor = amicus::cursor_state )
	{
		int y  = cursor->pointer[ 0 ];
		int x  = cursor->pointer[ 1 ];
		int dy = cursor->hotspot[ 0 ];
		int dx = cursor->hotspot[ 1 ];
		
		glfb::set_cursor_hotspot( dx, dy );
		glfb::set_cursor_location( x, y );
		glfb::set_cursor_visibility( cursor->visible );
		
		glfb::set_cursor_image( cursor );
	}
	
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
	                                      (void*) &load,
	                                      &keyboardHandler );
	
	EventHandlerRef modifiersHandler;
	err = InstallApplicationEventHandler( UPP_ARG( Modifiers_changed ),
	                                      LENGTH( Modifiers_event ),
	                                      Modifiers_event,
	                                      (void*) &load,
	                                      &modifiersHandler );
	
	mac::app::set_Aqua_menu_key( kHICommandQuit, '\0' );
	mac::app::set_Aqua_menu_key( kHICommandHide, '\0' );
	
	const Fixed max_zoom = maximum_zoom( desc.width, desc.height );
	
	const int shifted_max_zoom = max_zoom >> 15;
	
	for ( int i = 2 * 4;  i > shifted_max_zoom;  --i )
	{
		OSType command_ID = ('0' + (i >> 1)) << 24
		                  | "05"[ i & 1 ]    << 16
		                  | '0'              <<  8
		                  | '%';
		
		DisableMenuCommand( View, command_ID );
	}
	
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
