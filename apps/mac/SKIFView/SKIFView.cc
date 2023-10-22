/*
	SKIFView.cc
	-----------
*/

// Mac OS X
#ifdef __APPLE__
#include <Carbon/Carbon.h>
#endif

// Mac OS
#ifndef __MACWINDOWS__
#include <MacWindows.h>
#endif
#ifndef __RESOURCES__
#include <Resources.h>
#endif

// more-libc
#include "more/string.h"

// mac-config
#include "mac_config/apple-events.hh"
#include "mac_config/desk-accessories.hh"

// mac-sys-utils
#include "mac_sys/gestalt.hh"
#include "mac_sys/trap_available.hh"

// mac-qd-utils
#include "mac_qd/wide_drag_area.hh"

// mac-rsrc-utils
#include "mac_rsrc/get_string_from_resource.hh"

// mac-app-utils
#include "mac_app/about_box.hh"
#include "mac_app/DAs.hh"
#include "mac_app/event_handlers.hh"
#include "mac_app/init.hh"
#include "mac_app/menus.hh"
#include "mac_app/state.hh"

// SKIFView
#include "file_open.hh"
#include "window.hh"


#pragma exceptions off


#define STRLEN( s )  (sizeof "" s - 1)

#define STR_LEN( s )  "" s, (sizeof s - 1)

#define CONFIG_DAs CONFIG_DESK_ACCESSORIES

#if TARGET_API_MAC_CARBON
#define SystemTask()  /**/
#endif

using mac::qd::wide_drag_area;


extern "C" { int errno; }

static Str255 name;
static Str255 vers;
static Str255 copy;

/*
	Avoid the need to link with libc for memmove()
	by defining our own version.
*/

static inline
void MemMove( void* dst, const void* src, size_t n )
{
	return BlockMoveData( src, dst, n );
}

static
bool get_data()
{
	const OSType creator = 'SKIF';
	
	if ( Handle h = GetResource( creator, 0 ) )
	{
		GetResInfo( h, NULL, NULL, name );
		
		mac::rsrc::get_string_from_handle( copy, h );
	}
	
	mac::rsrc::get_vers_ShortVersionString( vers, 1 );
	
	if ( vers[ 0 ] <= 255 - STRLEN( "Version " ) )
	{
		MemMove( vers + 1 + STRLEN( "Version " ), vers + 1, vers[ 0 ] );
		mempcpy( vers + 1, STR_LEN( "Version " ) );
		
		vers[ 0 ] += STRLEN( "Version " );
	}
	
	mac::app::gAboutBoxAppName = name;
	mac::app::gAboutBoxAppVers = vers;
	mac::app::gAboutBoxAppCopy = copy;
	
	return true;
}

static
void close_window( WindowRef window )
{
	if ( ! mac::app::close_About_box( window ) )
	{
		destroy_window( window );
	}
}

static
void menu_item_chosen( long choice )
{
	short menu = choice >> 16;
	short item = choice;
	
	switch ( menu )
	{
		case 1:  // (Apple)
			if ( item == 1 )
			{
				// About...
				static bool got_data = get_data();
				
				mac::app::show_About_box();
			}
			else if ( ! TARGET_API_MAC_CARBON )
			{
				mac::app::open_DA_from_menu( item );
			}
			break;
		
		case 2:  // File
			switch ( item )
			{
				case 1:  // New
					break;
				
				case 2:  // Open
					file_open();
					break;
				
				case 3:  // Close
					if ( CONFIG_DAs  &&  mac::app::close_front_DA() )
					{
						// done
					}
					else if ( WindowRef window = FrontWindow() )
					{
						close_window( window );
					}
					break;
				
				case 4:  // -
				case 5:  // Quit
					mac::app::quitting = true;
					break;
				
				default:
					break;
			}
		
		case 3:  // Edit
			break;
		
		default:
			break;
	}
	
	HiliteMenu( 0 );
}

static inline
bool has_WaitNextEvent()
{
	enum { _WaitNextEvent = 0xA860 };
	
	return ! TARGET_CPU_68K  ||  mac::sys::trap_available( _WaitNextEvent );
}

static inline
Boolean WaitNextEvent( EventRecord& event )
{
	return WaitNextEvent( everyEvent, &event, 0x7FFFFFFF, NULL );
}

static
Boolean wait_next_event( EventRecord& event )
{
	SystemTask();
	
	return GetNextEvent( everyEvent, &event );
}

int main()
{
	using mac::app::quitting;
	
	mac::app::init_toolbox();
	mac::app::install_menus();
	
	// gestaltAppleEventsAttr = 'evnt'
	
	const bool apple_events_present =
		CONFIG_APPLE_EVENTS  &&
			(CONFIG_APPLE_EVENTS_GRANTED  ||
				mac::sys::gestalt( 'evnt' ) != 0);
	
	if ( apple_events_present )
	{
		mac::app::install_basic_event_handlers();
	}
	
	open_launched_documents();
	
	const bool has_WNE = has_WaitNextEvent();
	
	while ( ! quitting )
	{
		EventRecord event;
		
		if ( has_WNE ? WaitNextEvent( event ) : wait_next_event( event ) )
		{
			WindowRef window;
			
			switch ( event.what )
			{
				case mouseDown:
					switch ( FindWindow( event.where, &window ) )
					{
						case inMenuBar:
							menu_item_chosen( MenuSelect( event.where ) );
							break;
						
					#if CONFIG_DESK_ACCESSORIES
						
						case inSysWindow:
							SystemClick( &event, window );
							break;
						
					#endif
						
						case inDrag:
							DragWindow( window, event.where, wide_drag_area() );
							break;
						
						case inGoAway:
							if ( TrackGoAway( window, event.where ) )
							{
								close_window( window );
							}
							break;
						
						case inContent:
							SelectWindow( window );
							break;
						
						default:
							break;
					}
					break;
				
				case keyDown:
					if ( event.modifiers & cmdKey )
					{
						menu_item_chosen( MenuKey( event.message ) );
					}
					break;
				
				case updateEvt:
					window = (WindowRef) event.message;
					
					BeginUpdate( window );
					draw_window( window );
					EndUpdate  ( window );
					break;
				
			#if CONFIG_APPLE_EVENTS
				
				case kHighLevelEvent:
					(void) AEProcessAppleEvent( &event );
					break;
				
			#endif
				
				default:
					break;
			}
		}
	}
	
	return 0;
}
