/*
	NyanCat.cc
	----------
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
#include "missing/QDOffscreen.h"
#include "missing/Quickdraw.h"
#endif
#endif

// mac-config
#include "mac_config/apple-events.hh"

// mac-sys-utils
#include "mac_sys/gestalt.hh"

// mac-qd-utils
#include "mac_qd/get_portRect.hh"
#include "mac_qd/main_display_bounds.hh"
#include "mac_qd/wide_drag_area.hh"

// mac-app-utils
#include "mac_app/event_handlers.hh"
#include "mac_app/init.hh"
#include "mac_app/menus.hh"
#include "mac_app/state.hh"

// nyancatlib
#include "nyancat/graphics.hh"

// NyanCat
#include "Bitmap.hh"


using mac::qd::get_portRect;
using mac::qd::main_display_bounds;
using mac::qd::wide_drag_area;

using nyancat::bitmap;
using nyancat::n_frames;


// gestaltAppleEventsAttr = 'evnt'

const bool apple_events_present =
	CONFIG_APPLE_EVENTS  &&
		(CONFIG_APPLE_EVENTS_GRANTED  ||
			mac::sys::gestalt( 'evnt' ) != 0);

const Pattern veryDarkGray = { 0x77, 0xFF, 0xDD, 0xFF, 0x77, 0xFF, 0xDD, 0xFF };

const int zoom = 4;

const int nyan_width  = zoom * 70;
const int nyan_height = zoom * 72;

const int nyan_stride = (nyan_width + 15) / 16 * 2;

const int offscreen_buffer_size = nyan_stride * nyan_height * n_frames;

void bitmap::set_pixel( unsigned x, unsigned y, const Pattern& color )
{
	x *= its_magnifier;
	y *= its_magnifier;
	
	Rect r = { y, x, y + its_magnifier, x + its_magnifier };
	
	FillRect( &r, &color );
}

const int fps = 15;
const int tick_period = 60 / fps;

static Rect buffer_bounds = { 0, 0, nyan_height * n_frames, nyan_width };

static GrafPtr offscreen_port;

static
void make_offscreen_port()
{
	Ptr addr = NewPtr( offscreen_buffer_size );
	
	BitMap bitmap = { addr, nyan_stride, buffer_bounds };
	
#if ! TARGET_API_MAC_CARBON
	
	static GrafPort port;
	
	offscreen_port = &port;
	
	OpenPort( offscreen_port );
	
	RectRgn( offscreen_port->visRgn, &buffer_bounds );
	
	SetPortBits( &bitmap );
	
	PortSize( nyan_width, nyan_height * n_frames );
	
#else
	
	QDErr err = NewGWorld( &offscreen_port, 1, &buffer_bounds, NULL, NULL, 0 );
	
	if ( err )
	{
		ExitToShell();
	}
	
	SetGWorld( offscreen_port, NULL );
	
#endif
}

static
void make_offscreen_buffer()
{
	CursHandle cursor_handle = GetCursor( watchCursor );
	
	if ( cursor_handle )
	{
		Cursor cursor = **cursor_handle;
		
		SetCursor( &cursor );
	}
	
	make_offscreen_port();
	
	bitmap bits( zoom );
	
	FillRect( &buffer_bounds, &veryDarkGray );
	
	draw_frame( bits, 0 );
	
	for ( int i = 1;  i < n_frames;  ++i )
	{
		MovePortTo( 0, nyan_height * i );
		
		draw_frame( bits, i );
	}
	
	MovePortTo( 0, 0 );
	
	if ( cursor_handle )
	{
		InitCursor();
	}
}

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
	                         "\p" "Nyan Cat",
	                         true,
	                         noGrowDocProc,
	                         (WindowRef) -1,
	                         true,
	                         0 );
}

class timer
{
	private:
		uint32_t its_frame_duration;
		uint32_t its_time_of_play;
		int its_start_frame;
		bool it_is_paused;
	
	public:
		static const uint32_t arbitrarily_long = 0x7FFFFFFF;
		
		static uint32_t clock()  { return TickCount(); }
		
		timer() : its_start_frame(), it_is_paused()
		{
			its_frame_duration = tick_period;  // e.g. 4 (ticks) for 15fps
		}
		
		void play()  { it_is_paused = false;  its_time_of_play = clock(); }
		void pause()  { its_start_frame = frame();  it_is_paused = true; }
		
		void play_pause()
		{
			it_is_paused ? play() : pause();
		}
		
		void advance( int skip )  { its_start_frame += skip; }
		
		int frame() const;
		
		bool paused() const  { return it_is_paused; }
		
		uint32_t ticks_until_next_frame() const
		{
			if ( paused() )
			{
				return arbitrarily_long;
			}
			
			const uint32_t t = clock() - its_time_of_play;
			
			return its_frame_duration - t % its_frame_duration;
		}
};

int timer::frame() const
{
	if ( it_is_paused )
	{
		return its_start_frame;
	}
	
	const uint32_t now = clock();
	
	return (now - its_time_of_play) / its_frame_duration + its_start_frame;
}

static timer animation_timer;

static
unsigned next_sleep()
{
	return animation_timer.ticks_until_next_frame();
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

static
void draw_window( WindowRef window )
{
	SetPortWindowPort( window );
	
	unsigned t = animation_timer.frame();
	
	t %= 12;
	
	const Rect& portRect = get_portRect( window );
	
	Rect srcRect = portRect;
	
	OffsetRect( &srcRect, 0, t * nyan_height );
	
	CopyBits( GetPortBitMapForCopyBits( (CGrafPtr) offscreen_port ),
	          GetPortBitMapForCopyBits( GetWindowPort( window ) ),
	          &srcRect,
	          &portRect,
	          srcCopy,
	          NULL );
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
	
	make_offscreen_buffer();
	
	make_main_window();
	
	animation_timer.play();
	
	static int last_frame = -1;
	
	while ( ! quitting )
	{
		if ( main_window )
		{
			int next_frame = animation_timer.frame() % 12;
			
			if ( next_frame != last_frame )
			{
				draw_window( main_window );
			}
		}
		
		EventRecord event;
		
		if ( WaitNextEvent( everyEvent, &event, next_sleep(), NULL ) )
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
								DisposeWindow( main_window );
								
								main_window = NULL;
								quitting = true;
							}
							break;
						
						case inContent:
							animation_timer.play_pause();
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
							animation_timer.play_pause();
							break;
						
						case 0x1C:
							animation_timer.advance( -1 );
							break;
						
						case 0x1D:
							animation_timer.advance( 1 );
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
