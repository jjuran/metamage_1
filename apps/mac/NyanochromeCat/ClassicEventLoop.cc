/*
	ClassicEventLoop.cc
	-------------------
*/

#include "ClassicEventLoop.hh"

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
#include "mac_sys/trap_available.hh"

// mac-qd-utils
#include "mac_qd/wide_drag_area.hh"

// mac-app-utils
#include "mac_app/DAs.hh"
#include "mac_app/menus.hh"
#include "mac_app/state.hh"

// NyanochromeCat
#include "About.hh"
#include "Magic.hh"
#include "Offscreen.hh"
#include "Timer.hh"
#include "Window.hh"


#pragma exceptions off


#define CONFIG_DAs CONFIG_DESK_ACCESSORIES

#if CALL_NOT_IN_CARBON
	#define TrackGoAway( w, pt )  TrackGoAway_magic( w, pt )
	#define DragWindow( w, pt, r )  DragWindow_magic( w, pt, r )
#else
	#define SystemTask()  /**/
#endif  // #if CALL_NOT_IN_CARBON

using mac::qd::wide_drag_area;


static
void play_pause()
{
	animation_timer.paused() ? animation_timer.play()
	                         : animation_timer.pause();
}

static
unsigned next_sleep()
{
	using monotonic_clock::ticks_from;
	
	return ticks_from( animation_timer.clocks_until_next_pulse() );
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
				show_About_box();
			}
			else if ( ! TARGET_API_MAC_CARBON )
			{
				mac::app::open_DA_from_menu( item );
			}
			
			SetPortWindowPort( main_window );
			break;
		
		case 2:  // File
			switch ( item )
			{
				case 1:  // New
				case 2:  // Open
					break;
				
				case 3:  // Close
					if ( CONFIG_DAs  &&  mac::app::close_front_DA() )
					{
						break;
					}
					
					if ( close_About_box( FrontWindow() ) )
					{
						break;
					}
				
				case 4:  // -
				case 5:  // Quit
					mac::app::quitting = true;
					break;
				
				default:
					break;
			}
		
		case 3:  // Edit
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
	return WaitNextEvent( everyEvent, &event, next_sleep(), NULL );
}

static inline
Boolean GetNextEvent( EventRecord& event )
{
	SystemTask();
	
	return GetNextEvent( everyEvent, &event );
}

void classic_event_loop()
{
	using mac::app::quitting;
	
	const bool has_WNE = has_WaitNextEvent();
	
	while ( ! quitting )
	{
		if ( main_window )
		{
			if ( animation_timer.pulse() )
			{
				prepare_next_frame();
				blit_window( main_window );
			}
		}
		
		EventRecord event;
		
		if ( has_WNE ? WaitNextEvent( event ) : GetNextEvent( event ) )
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
								if ( ! close_About_box( window ) )
								{
									DisposeWindow( main_window );
									
									main_window = NULL;
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
							
							if ( window == main_window )
							{
								play_pause();
							}
							break;
						
						default:
							break;
					}
					break;
					
				case mouseUp:
					break;
					
				case keyDown:
					if ( event.modifiers & cmdKey )
					{
						menu_item_chosen( MenuKey( event.message ) );
					}
					
					switch( (char) event.message )
					{
						case ' ':
							play_pause();
							break;
						
						case 0x1C:
							prepare_prev_frame();
							blit_window( main_window );
							break;
						
						case 0x1D:
							prepare_next_frame();
							blit_window( main_window );
							break;
						
						default:
							break;
					}
					break;
				
				case keyUp:
					break;
				
				case updateEvt:
					window = (WindowRef) event.message;
					
					BeginUpdate( window );
					
					if ( window != main_window )
					{
						SetPortWindowPort( window );
						
						draw_About_box();
						
						SetPortWindowPort( main_window );
					}
					else
					{
						draw_window( window );
					}
					
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
}
