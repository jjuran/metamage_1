/*
	NyanochromeCat.cc
	-----------------
*/

// Mac OS X
#ifdef __APPLE__
#include <Carbon/Carbon.h>
#endif

// Mac OS
#ifndef __MACWINDOWS__
#include <MacWindows.h>
#endif

// missing-macos
#ifdef MAC_OS_X_VERSION_10_7
#ifndef MISSING_QUICKDRAW_H
#include "missing/Quickdraw.h"
#endif
#endif

// mac-config
#include "mac_config/apple-events.hh"
#include "mac_config/desk-accessories.hh"

// mac-sys-utils
#include "mac_sys/clock.hh"
#include "mac_sys/gestalt.hh"
#include "mac_sys/trap_available.hh"

// mac-qd-utils
#include "mac_qd/main_display_bounds.hh"
#include "mac_qd/wide_drag_area.hh"

// mac-app-utils
#include "mac_app/DAs.hh"
#include "mac_app/event_handlers.hh"
#include "mac_app/init.hh"
#include "mac_app/menus.hh"
#include "mac_app/state.hh"

// NyanochromeCat
#include "Geometry.hh"
#include "Offscreen.hh"


#define CONFIG_DAs CONFIG_DESK_ACCESSORIES

#if TARGET_API_MAC_CARBON
#define SystemTask()  /**/
#endif

using mac::qd::main_display_bounds;
using mac::qd::wide_drag_area;


// gestaltAppleEventsAttr = 'evnt'

const bool apple_events_present =
	CONFIG_APPLE_EVENTS  &&
		(CONFIG_APPLE_EVENTS_GRANTED  ||
			mac::sys::gestalt( 'evnt' ) != 0);

#ifdef __MC68K__
	
	namespace monotonic_clock = mac::sys::tick_clock;
	
#else
	
	namespace monotonic_clock = mac::sys::microsecond_clock;
	
#endif

const int fps = 15;

const monotonic_clock::clock_t clock_period =
	monotonic_clock::clocks_per_kilosecond / 1000 / fps;

static inline
short aligned( short x, short alignment )
{
	const short antimask = alignment - 1;
	const short rounding = antimask / 2;
	
	return x + rounding & ~antimask;
}

static WindowRef main_window;

static
void make_main_window()
{
	const short alignment = 16;
	
	Rect bounds = main_display_bounds();
	
	const short exact_left = (bounds.left + bounds.right - nyan_width ) / 2;
	
	const short aligned_left = aligned( exact_left, alignment );
	
	bounds.left  = aligned_left;
	bounds.right = bounds.left + nyan_width;
	
	bounds.top    = (bounds.top + bounds.bottom - nyan_height - 41) / 2 + 39;
	bounds.bottom = bounds.top + nyan_height;
	
	main_window = NewWindow( NULL,
	                         &bounds,
	                         "\p" "Nyanochrome Cat",
	                         true,
	                         noGrowDocProc,
	                         (WindowRef) -1,
	                         true,
	                         0 );
	
	SetPortWindowPort( main_window );
}

class timer
{
	private:
		typedef monotonic_clock::clock_t clock_t;
		
		clock_t its_pulse_interval;
		clock_t its_next_pulse;
		bool it_is_paused;
	
	public:
		static const clock_t arbitrarily_long = clock_t( -1 );
		
		static void get_clock( clock_t* t )  { monotonic_clock::get( t ); }
		
		timer() : its_next_pulse(), it_is_paused( true )
		{
			its_pulse_interval = clock_period;  // e.g. 4 ticks for 15fps
		}
		
		void start()  { play();  its_next_pulse += its_pulse_interval; }
		
		void play()  { it_is_paused = false;  get_clock( &its_next_pulse ); }
		void pause()  { it_is_paused = true; }
		
		bool pulse();
		
		bool paused() const  { return it_is_paused; }
		
		clock_t clocks_until_next_pulse() const
		{
			if ( paused() )
			{
				return arbitrarily_long;
			}
			
			clock_t t;
			get_clock( &t );
			
			if ( t >= its_next_pulse )
			{
				return 0;
			}
			
			return its_next_pulse - t;
		}
};

bool timer::pulse()
{
	if ( it_is_paused )
	{
		return false;
	}
	
	clock_t now;
	get_clock( &now );
	
	if ( now >= its_next_pulse )
	{
		its_next_pulse += its_pulse_interval;
		
		if ( its_next_pulse <= now )
		{
			its_next_pulse = now + its_pulse_interval;
		}
		
		return true;
	}
	
	return false;
}

static timer animation_timer;

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
				// About...
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
				case 2:  // Open
					break;
				
				case 3:  // Close
					if ( CONFIG_DAs  &&  mac::app::close_front_DA() )
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
void draw_window( WindowRef window )
{
	blit( GetWindowPort( window ) );
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

int main()
{
	using mac::app::quitting;
	
	mac::app::init_toolbox();
	mac::app::install_menus();
	
	if ( apple_events_present )
	{
		mac::app::install_basic_event_handlers();
	}
	
	SetEventMask( everyEvent );
	
	CursHandle cursor_handle = GetCursor( watchCursor );
	
	if ( cursor_handle )
	{
		Cursor cursor = **cursor_handle;
		
		SetCursor( &cursor );
	}
	
	render_offscreen();
	
	if ( cursor_handle )
	{
		InitCursor();
	}
	
	make_main_window();
	
	draw_window( main_window );
	
	animation_timer.start();
	
	const bool has_WNE = has_WaitNextEvent();
	
	while ( ! quitting )
	{
		if ( main_window )
		{
			if ( animation_timer.pulse() )
			{
				prepare_next_frame();
				draw_window( main_window );
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
								DisposeWindow( main_window );
								
								main_window = NULL;
								quitting = true;
							}
							break;
						
						case inContent:
							if ( CONFIG_DAs  &&  window != FrontWindow() )
							{
								SelectWindow( window );
								break;
							}
							
							play_pause();
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
							draw_window( main_window );
							break;
						
						case 0x1D:
							prepare_next_frame();
							draw_window( main_window );
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
	
	ExitToShell();
	
	return 0;
}
