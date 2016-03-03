#include <Sound.h>

static QDGlobals qd;

int main()
{
	Boolean quitting = false;
	Boolean inverted_menu_bar = false;
	SInt32 dummy;
	
	InitGraf( &qd.thePort );
	
	InitFonts();
	InitWindows();
	InitMenus();
	
	while ( ! quitting )
	{
		EventRecord event;
		
		if ( GetNextEvent( everyEvent, &event ) )
		{
			switch ( event.what )
			{
				case mouseDown:
					if ( ! inverted_menu_bar )
					{
						inverted_menu_bar = true;
						
						FlashMenuBar( 0 );
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
	
	Delay( 30, (UInt32*) &dummy );
	
	ExitToShell();
	
	return 0;
}
