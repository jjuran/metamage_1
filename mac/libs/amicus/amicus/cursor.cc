/*
	cursor.cc
	---------
*/

#include "amicus/cursor.hh"

// Mac OS X
#ifdef __APPLE__
// Including just <CoreGraphics/CoreGraphics.h> fails in Mac OS X 10.4.
#include <ApplicationServices/ApplicationServices.h>
#endif


namespace amicus
{

bool cursor_hidden;

void set_cursor_hidden( bool hidden )
{
	if ( hidden != cursor_hidden )
	{
		cursor_hidden = hidden;
		
		if ( hidden )
		{
			CGDisplayHideCursor( 0 );
		}
		else
		{
			CGDisplayShowCursor( 0 );
		}
	}
}

}
