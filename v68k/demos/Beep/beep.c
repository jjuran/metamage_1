#include <Sound.h>

static QDGlobals qd;

int main()
{
	SInt32 dummy;
	
	InitGraf( &qd.thePort );
	
	InitFonts();
	InitWindows();
	InitMenus();
	
	SysBeep( 30 );
	
	Delay( 30, (UInt32*) &dummy );
	
	return 0;
}
