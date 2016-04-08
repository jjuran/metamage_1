/*
	test.cc
	-------
*/

// Mac OS X
#ifdef __APPLE__
#include <QuickTime/QuickTime.h>
#endif

// Mac OS
#ifndef __LOWMEM__
#include <LowMem.h>
#endif
#ifndef __MOVIES__
#include <Movies.h>
#endif
#ifndef __SOUND__
#include <Sound.h>
#endif

#if ! TARGET_API_MAC_CARBON

static QDGlobals qd;

static
void set_desktop_pattern( const Pattern* pattern )
{
	LMSetDeskPattern( pattern );
	
	PaintOne( NULL, LMGetWMgrPort()->visRgn );
}

#endif

static
void clobber_screen()
{
#if ! TARGET_API_MAC_CARBON
	
	GrafPort port;
	
	GrafPtr saved_port;
	GetPort( &saved_port );
	
	OpenPort( &port );
	
	FillRect( &port.portRect, &qd.black );
	
	ClosePort( &port );
	
	SetPort( saved_port );
	
#endif
}

static
void refresh_screen()
{
	DrawMenuBar();
	
#if ! TARGET_API_MAC_CARBON
	
	PaintBehind( NULL, LMGetGrayRgn() );
	
#endif
}

static Ptr fullscreen_context;

static
void enter_fullscreen()
{
	if ( TARGET_API_MAC_CARBON )
	{
		if ( fullscreen_context == NULL )
		{
			WindowRef window;
			
			OSErr err = BeginFullScreen( &fullscreen_context,
			                             NULL,  // gd
			                             NULL,  // width
			                             NULL,  // height
			                             &window,
			                             NULL,  // color
			                             0 );
		}
	}
	else
	{
		clobber_screen();
	}
}

static
void leave_fullscreen()
{
	if ( TARGET_API_MAC_CARBON )
	{
		if ( fullscreen_context != NULL )
		{
			EndFullScreen( fullscreen_context, 0 );
			
			fullscreen_context = NULL;
		}
	}
	else
	{
		refresh_screen();
	}
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
						
					#if ! TARGET_API_MAC_CARBON
						
						case '1': set_desktop_pattern( &qd.white  );  break;
						case '2': set_desktop_pattern( &qd.ltGray );  break;
						case '3': set_desktop_pattern( &qd.gray   );  break;
						case '4': set_desktop_pattern( &qd.dkGray );  break;
						case '5': set_desktop_pattern( &qd.black  );  break;
						
					#endif
						
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
	
	ExitToShell();
	
	return 0;
}
