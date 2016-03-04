/*
	test.c
	------
*/

// Mac OS X
#ifdef __APPLE__
#include <Carbon/Carbon.h>
#endif

// Mac OS
#ifndef __SOUND__
#include <Sound.h>
#endif

#if ! TARGET_API_MAC_CARBON

static QDGlobals qd;

#endif

int main()
{
	Boolean quitting = false;
	Boolean inverted_menu_bar = false;
	
#if ! TARGET_API_MAC_CARBON
	
	InitGraf( &qd.thePort );
	
	InitFonts();
	InitWindows();
	InitMenus();
	
#endif
	
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
							if ( ! inverted_menu_bar )
							{
								inverted_menu_bar = true;
								
								FlashMenuBar( 0 );
							}
							break;
						
						default:
							break;
					}
					break;
					
				case mouseUp:
					if ( inverted_menu_bar )
					{
						inverted_menu_bar = false;
						
						FlashMenuBar( 0 );
					}
					break;
					
				case keyDown:
					if ( (char) event.message == 'q' )
					{
						quitting = true;
					}
					else
					{
						SysBeep( 30 );
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
