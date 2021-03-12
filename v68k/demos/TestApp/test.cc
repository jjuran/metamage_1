/*
	test.cc
	-------
*/

// Mac OS X
#ifdef __APPLE__
#include <Carbon/Carbon.h>
#endif

// Mac OS
#ifndef __LOWMEM__
#include <LowMem.h>
#endif
#ifndef __SOUND__
#include <Sound.h>
#endif

// missing-macos
#ifdef MAC_OS_X_VERSION_10_7
#ifndef MISSING_QUICKDRAW_H
#include "missing/Quickdraw.h"
#endif
#endif

// mac-sys-utils
#include "mac_sys/trap_available.hh"

// Nostalgia
#include "Nostalgia/MacWindows.hh"

// TestApp
#include "desktop.hh"
#include "display.hh"
#include "fullscreen.hh"
#include "fullscreen_port.hh"


#if TARGET_API_MAC_CARBON
#define SystemTask()  /**/
#endif

static const Rect grow_size =
{
	50, 50,
	32767, 32767,
};

static
Point get_window_topLeft( WindowRef w )
{
#if ! TARGET_API_MAC_CARBON
	
	const Rect& bitmap_bounds = w->portBits.bounds;
	
	Point topLeft = { -bitmap_bounds.top, -bitmap_bounds.left };
	
	return topLeft;
	
#endif
	
	Rect bounds;
	
	GetWindowBounds( w, kWindowContentRgn, &bounds );
	
	return *(Point*) &bounds;
}

static
void move_front_window( short dh, short dv )
{
	if ( WindowRef front = FrontWindow() )
	{
		Point topLeft = get_window_topLeft( front );
		
		topLeft.h += dh;
		topLeft.v += dv;
		
		MoveWindow( front, topLeft.h, topLeft.v, false );
	}
}

static
const Rect* drag_bounds()
{
#if ! TARGET_API_MAC_CARBON
	
	return &LMGetGrayRgn()[0]->rgnBBox;
	
#endif
	
	return NULL;  // DragWindow() bounds may be NULL in Carbon
}

#if ! TARGET_API_MAC_CARBON

static inline
const Rect& get_window_portRect( WindowRef window )
{
	return window->portRect;
}

#else

static inline
Rect get_window_portRect( WindowRef window )
{
	Rect bounds;
	GetPortBounds( GetWindowPort( window ), &bounds );
	
	return bounds;
}

#endif

static
void invalidate_scroll_bar_areas( WindowRef window )
{
	const Rect& portRect = get_window_portRect( window );
	
	const short right  = portRect.right;
	const short bottom = portRect.bottom;
	
	const Rect vstrip = { 0, right - 15,  bottom, right };
	const Rect hstrip = { bottom - 15, 0, bottom, right };
	
	InvalRect( &vstrip );
	InvalRect( &hstrip );
}

static inline
void EraseRect( const Rect& rect )
{
	EraseRect( &rect );
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

static inline
Boolean GetNextEvent( EventRecord& event )
{
	SystemTask();
	
	return GetNextEvent( everyEvent, &event );
}

int main()
{
	Boolean quitting = false;
	int menu_bar_inversion_level = 0;
	
#if ! TARGET_API_MAC_CARBON
	
	InitGraf( &qd.thePort );
	
	InitFonts();
	InitWindows();
	InitMenus();
	
	InitCursor();
	
#endif
	
	SetEventMask( everyEvent );
	
	WindowRef main_window = NULL;
	
	const bool has_WNE = has_WaitNextEvent();
	
	while ( ! quitting )
	{
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
							if ( menu_bar_inversion_level-- == 0 )
							{
								FlashMenuBar( 0 );
							}
							break;
						
						case inDrag:
							DragWindow( window, event.where, drag_bounds() );
							break;
						
						case inGrow:
							if ( long grown = GrowWindow( window, event.where, &grow_size ) )
							{
								invalidate_scroll_bar_areas( window );
								
								short h = grown;
								short v = grown >> 16;
								
								SizeWindow( window, h, v, true );
								
								invalidate_scroll_bar_areas( window );
							}
							break;
						
						case inGoAway:
							if ( TrackGoAway( window, event.where ) )
							{
								DisposeWindow( main_window );
								
								main_window = NULL;
							}
						
						default:
							break;
					}
					break;
					
				case mouseUp:
					if ( ++menu_bar_inversion_level == 0 )
					{
						FlashMenuBar( 0 );
					}
					break;
					
				case keyDown:
					switch( (char) event.message )
					{
						case 0x08:  enter_fullscreen();  break;  // Delete
						case 0x1B:  leave_fullscreen();  break;  // Esc
						
						case 0x1C:  move_front_window( -8, 0 );  break;
						case 0x1D:  move_front_window(  8, 0 );  break;
						case 0x1E:  move_front_window( 0, -8 );  break;
						case 0x1F:  move_front_window( 0,  8 );  break;
						
					#if ! TARGET_API_MAC_CARBON
						
						case '1': set_desktop_pattern( qd.white  );  break;
						case '2': set_desktop_pattern( qd.ltGray );  break;
						case '3': set_desktop_pattern( qd.gray   );  break;
						case '4': set_desktop_pattern( qd.dkGray );  break;
						case '5': set_desktop_pattern( qd.black  );  break;
						
					#endif
						
						case 'n':
							if ( main_window == NULL )
							{
								Rect bounds = main_display_bounds();
								
								bounds.left  += bounds.right / 4;
								bounds.right -= bounds.left;
								
								bounds.top    += bounds.bottom / 4;
								bounds.bottom -= bounds.top;
								
								main_window = NewWindow( NULL,
								                         &bounds,
								                         "\p",
								                         true,
								                         documentProc,
								                         (WindowRef) -1,
								                         true,
								                         0 );
							}
							break;
						
						case 'w':
							if ( main_window != NULL )
							{
								DisposeWindow( main_window );
								
								main_window = NULL;
							}
							break;
						
						case 'q':
							quitting = true;
							break;
						
						case ' ':
							if ( menu_bar_inversion_level-- == 0 )
							{
								FlashMenuBar( 0 );
							}
							break;
						
						default:
							SysBeep( 30 );
							break;
					}
					break;
				
				case keyUp:
					if ( (char) event.message == ' ' )
					{
						if ( ++menu_bar_inversion_level == 0 )
						{
							FlashMenuBar( 0 );
						}
					}
					break;
				
				case updateEvt:
					window = (WindowRef) event.message;
					BeginUpdate( window );
					
					SetPortWindowPort( window );
					
					EraseRect( get_window_portRect( window ) );
					
					DrawGrowIcon( window );
					
					EndUpdate  ( window );
					break;
				
				default:
					break;
			}
		}
	}
	
	if ( ! TARGET_API_MAC_CARBON )
	{
		cleanup_screen();
	}
	
	ExitToShell();
	
	return 0;
}
