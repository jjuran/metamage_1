/*
	SoundCheck.cc
	-------------
	
	Sound Check for classic Mac OS
	
	Copyright 2025, Joshua Juran.  All rights reserved.
	
	License:  AGPLv3+ (see bottom for legal boilerplate)
	
	Sound Check tests the Sound Driver API.
	
	This module defines the main event loop.
	
*/

// Mac OS
#ifndef __MACWINDOWS__
#include <MacWindows.h>
#endif
#ifndef __MENUS__
#include <Menus.h>
#endif

// mac-config
#include "mac_config/apple-events.hh"
#include "mac_config/desk-accessories.hh"

// mac-sys-utils
#include "mac_sys/gestalt.hh"

// mac-qd-utils
#include "mac_qd/wide_drag_area.hh"

// mac-app-utils
#include "mac_app/about_box.hh"
#include "mac_app/DAs.hh"
#include "mac_app/event_handlers.hh"
#include "mac_app/menus.hh"
#include "mac_app/state.hh"
#include "mac_app/wait_next_event.hh"

// SoundCheck
#include "ExitToShell.hh"
#include "playback.hh"
#include "UI.hh"
#include "window.hh"


#pragma exceptions off


#define CONFIG_DAs CONFIG_DESK_ACCESSORIES

using mac::qd::wide_drag_area;


// gestaltAppleEventsAttr = 'evnt'

const bool apple_events_present =
	CONFIG_APPLE_EVENTS  &&
		(CONFIG_APPLE_EVENTS_GRANTED  ||
			mac::sys::gestalt( 'evnt' ) != 0);

static
void menu_item_chosen( long choice )
{
	short menu = choice >> 16;
	short item = choice;
	
	switch ( menu )
	{
		case Apple:
			if ( item == About )
			{
				// About...
				const OSType creator = 'SDck';
				
				mac::app::show_About_box( creator );
			}
			else if ( ! TARGET_API_MAC_CARBON )
			{
				mac::app::open_DA_from_menu( item );
			}
			break;
		
		case File:
			switch ( item )
			{
				case Test:
					play();
					break;
				
				case Stop:
					stop();
					break;
				
				case Close:
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
				
				case Quit:
					mac::app::quitting = true;
					break;
				
				default:
					break;
			}
		
		default:
			break;
	}
	
	HiliteMenu( 0 );
}

int main()
{
	using mac::app::quitting;
	using mac::app::wait_next_event;
	
	InitGraf( &qd.thePort );
	
	InitFonts();
	InitWindows();
	InitMenus();
	
	InitCursor();
	
	mac::app::install_menus();
	
	if ( apple_events_present )
	{
		mac::app::install_basic_event_handlers();
	}
	
	patch_ExitToShell();
	
	create_window();
	
	while ( ! quitting )
	{
		EventRecord event;
		
		if ( wait_next_event( event ) )
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
						
						case inDrag:
							DragWindow( window, event.where, wide_drag_area() );
							break;
						
						case inGoAway:
							if ( TrackGoAway( window, event.where ) )
							{
								if ( ! mac::app::close_About_box( window ) )
								{
									quitting = true;
								}
							}
							break;
						
						case inContent:
							if ( window != FrontWindow() )
							{
								SelectWindow( window );
								break;
							}
							
							GlobalToLocal( &event.where );
							
							content_click( window, event.where );
							
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
					else if ( FrontWindow() == console_window )
					{
						if ( (Byte) event.message == kReturnCharCode )
						{
							default_button_click();
						}
					}
					
					break;
				
				case updateEvt:
					window = (WindowRef) event.message;
					
					SetPortWindowPort( window );
					
					BeginUpdate( window );
					draw_window( window );
					EndUpdate  ( window );
					break;
				
				case kHighLevelEvent:
					(void) AEProcessAppleEvent( &event );
					break;
				
				default:
					break;
			}
		}
		
		if ( updated  &&  ! playing )
		{
			updated = false;
			
			stop();
		}
	}
	
	return 0;
}

/*
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/
