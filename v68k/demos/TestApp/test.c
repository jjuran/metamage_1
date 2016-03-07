/*
	test.c
	------
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

#if ! TARGET_API_MAC_CARBON

static QDGlobals qd;

#endif

static
void set_desktop_pattern( const Pattern* pattern )
{
	LMSetDeskPattern( pattern );
	
	PaintOne( NULL, LMGetWMgrPort()->visRgn );
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
						case '1': set_desktop_pattern( &qd.white  );  break;
						case '2': set_desktop_pattern( &qd.ltGray );  break;
						case '3': set_desktop_pattern( &qd.gray   );  break;
						case '4': set_desktop_pattern( &qd.dkGray );  break;
						case '5': set_desktop_pattern( &qd.black  );  break;
						
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
