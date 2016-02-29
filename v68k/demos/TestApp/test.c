#include <Sound.h>

static QDGlobals qd;

int main()
{
	Boolean quitting = false;
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
				case mouseUp:
					FlashMenuBar( 0 );
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
