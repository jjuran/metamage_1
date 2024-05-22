/*
	LegacynthServer.cc
	------------------
	
	Legacynth Server for classic Mac OS
	
	Copyright 2022-2024, Joshua Juran.  All rights reserved.
	
	License:  AGPLv3+ (see bottom for legal boilerplate)
	
	Legacynth implements the Sound Driver API in traditional Mac OS
	environments that don't.
	
	This application is a sound server and API adapter, converting
	I/O requests to the Sound Driver into Sound Manager commands.
	
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

// mac-sys-utils
#include "mac_sys/gestalt.hh"

// mac-qd-utils
#include "mac_qd/wide_drag_area.hh"

// mac-ui-utils
#include "mac_ui/windows.hh"

// mac-app-utils
#include "mac_app/about_box.hh"
#include "mac_app/event_handlers.hh"
#include "mac_app/menus.hh"
#include "mac_app/state.hh"
#include "mac_app/wait_next_event.hh"

// Legacynth
#include "ExitToShell.hh"
#include "Sound.hh"
#include "status.hh"
#include "UI.hh"
#include "window.hh"


#pragma exceptions off


#define LOWMEM( addr, type )  (*(type*) (addr))

#define SysVersion  LOWMEM( 0x015A, short )


using mac::qd::wide_drag_area;


// gestaltAppleEventsAttr = 'evnt'

const bool apple_events_present =
	CONFIG_APPLE_EVENTS  &&
		(CONFIG_APPLE_EVENTS_GRANTED  ||
			mac::sys::gestalt( 'evnt' ) != 0);

static inline
bool has_System_7()
{
	return ! TARGET_CPU_68K  ||  SysVersion >= 0x0700;
}

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
				const OSType creator = 'Snd*';
				
				mac::app::show_About_box( creator );
			}
			else if ( ! TARGET_API_MAC_CARBON  &&  has_System_7() )
			{
				mac::app::open_DA_from_menu( item );
			}
			break;
		
		case File:
			switch ( item )
			{
				case Start:
					startup();
					break;
					
				case Stop:
					cleanup();
					break;
				
				case Close:
					/*
						We don't open desk accessories before System 7,
						so we don't have to deal with their windows.
					*/
					
					if ( WindowRef window = FrontWindow() )
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
	
	OSErr err = Sound_installed();
	
	if ( err == noErr )
	{
		MenuRef fileMenu = GetMenuHandle( File );
		
		EnableItem( fileMenu, Start );
		
		patch_ExitToShell();
	}
	
	create_window( err );
	
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
					
					break;
				
				case updateEvt:
					window = (WindowRef) event.message;
					
					SetPortWindowPort( window );
					
					BeginUpdate( window );
					draw_window( window );
					EndUpdate  ( window );
					break;
				
				case osEvt:
					switch ( (event.message & osEvtMessageMask) >> 24 )
					{
						case suspendResumeMessage:
							mac::ui::invalidate_window( console_window );
							break;
						
						default:
							break;
					}
				
					break;
				
				case kHighLevelEvent:
					(void) AEProcessAppleEvent( &event );
					break;
				
				default:
					break;
			}
		}
		else if ( sound_changed )
		{
			sound_changed = false;
			
			mac::ui::invalidate_window( console_window );
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
