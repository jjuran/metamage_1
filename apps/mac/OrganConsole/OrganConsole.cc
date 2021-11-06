/*
	OrganConsole.cc
	---------------
*/

// Mac OS X
#ifdef __APPLE__
#include <Carbon/Carbon.h>
#endif

// Mac OS
#ifndef __MACWINDOWS__
#include <MacWindows.h>
#endif

// mac-config
#include "mac_config/apple-events.hh"
#include "mac_config/desk-accessories.hh"

// mac-sys-utils
#include "mac_sys/gestalt.hh"
#include "mac_sys/trap_available.hh"

// mac-qd-utils
#include "mac_qd/wide_drag_area.hh"

// mac-app-utils
#include "mac_app/about_box.hh"
#include "mac_app/DAs.hh"
#include "mac_app/event_handlers.hh"
#include "mac_app/init.hh"
#include "mac_app/menus.hh"
#include "mac_app/scoped_EventMask.hh"
#include "mac_app/state.hh"

// OrganConsole
#include "fourtone.hh"
#include "keymap.hh"
#include "rate.hh"
#include "soundgen.hh"
#include "window.hh"


#pragma exceptions off


#define CONFIG_DAs CONFIG_DESK_ACCESSORIES

#if TARGET_API_MAC_CARBON
#define SystemTask()  /**/
#endif

using mac::qd::wide_drag_area;

using mac::app::scoped_EventMask;


static inline
void acquire_key( unsigned char keycode )
{
	if ( short keydef = key_def( keycode ) )
	{
		const MIDI_note midi_note = keydef - 256;
		
		const FTRate rate = rate_from_midi_note( midi_note );
		
		fourtone_acquire_key_note( keycode, rate );
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
				const OSType creator = '|gan';
				
				mac::app::show_About_box( creator );
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
					break;
				
				case 3:  // Close
					if ( CONFIG_DAs  &&  mac::app::close_front_DA() )
					{
						// done
					}
					else if ( WindowRef window = FrontWindow() )
					{
						if ( mac::app::close_About_box( window ) )
						{
							break;
						}
					}
					// fall through
				
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
	
	scoped_EventMask eventMask( everyEvent );
	
	const bool has_WNE = has_WaitNextEvent();
	
	create_window();
	
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
								if ( ! mac::app::close_About_box( window ) )
								{
									mac::app::quitting = true;
								}
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
					else if ( (Byte) event.message == kBackspaceCharCode )
					{
						full_stop();
					}
					else
					{
						acquire_key( (UInt16) event.message >> 8 );
					}
					break;
				
				case keyUp:
					fourtone_release_not_down();
					break;
				
				case updateEvt:
					window = (WindowRef) event.message;
					
					BeginUpdate( window );
					
					if ( GetWindowKind( window ) != mac::app::kAboutWindowKind )
					{
						draw_window( window );
					}
					
					EndUpdate( window );
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
	
	full_stop();
	
	return 0;
}
