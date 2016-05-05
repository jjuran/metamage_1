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

// TestApp
#include "desktop.hh"
#include "display.hh"
#include "fullscreen.hh"
#include "fullscreen_port.hh"


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

int main()
{
	Boolean quitting = false;
	int menu_bar_inversion_level = 0;
	
#if ! TARGET_API_MAC_CARBON
	
	InitGraf( &qd.thePort );
	
	InitFonts();
	InitWindows();
	InitMenus();
	
#endif
	
	SetEventMask( everyEvent );
	
	WindowRef main_window = NULL;
	
	while ( ! quitting )
	{
		EventRecord event;
		
		if ( WaitNextEvent( everyEvent, &event, 0x7FFFFFFF, NULL ) )
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
								                         noGrowDocProc,
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
