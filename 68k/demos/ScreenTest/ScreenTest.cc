/*
	ScreenTest.cc
	-------------
	
	ScreenTest application for Advanced Mac Substitute
	
	This program fills the screen with a pattern.[1]  It does so
	by writing directly to screen memory, bypassing QuickDraw
	entirely.  The same bit pattern is written without regard for
	pixel format, and hence will appear differently depending on
	the bit depth and oter details of the screen.
	
	[1] It's not an 8x8 Pattern in the QuickDraw sense.  The base
	pattern is one byte wide and 256 rows tall, tiled to fill the
	entire screen.  Then, every 255th byte of screen memory is
	perturbed to create a further diagonal line pattern (by which
	we avoid the undesirable scenario of 32-bit formats being
	indistinguishable from one another or even 8-bit grayscale).
	
	The purpose of this program is to test whether the various
	front ends used with Advanced Mac Substitute support the
	relevant pixel formats, without requiring an implementation
	of Color QuickDraw (or even Graphics Devices).
	
*/

// Mac OS
#ifndef __EVENTS__
#include <Events.h>
#endif
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif

// ams-common
#include "callouts.hh"


#define LOWMEM( addr, type )  (*(type*) (addr))

#define ScrnBase  LOWMEM( 0x0824, Ptr   )
#define CrsrPin   LOWMEM( 0x0834, Rect  )
#define DeepRow   LOWMEM( 0x08AC, short )


int main()
{
	InitCursor();
	HideCursor();
	
	Ptr p = ScrnBase;
	
	Byte c = 0;
	
	for ( int y = 0;  y < CrsrPin.bottom;  ++y )
	{
		fast_memset( p, c++, DeepRow );
		
		p += DeepRow;
	}
	
	Ptr q = ScrnBase;
	
	while ( q < p )
	{
		*q ^= 0xED;
		
		q += 255;
	}
	
	ShowCursor();
	
	while ( true )
	{
		EventRecord event;
		
		if ( WaitNextEvent( everyEvent, &event, 0x7FFFFFFF, NULL ) )
		{
			switch ( event.what )
			{
				case mouseDown:
				case keyDown:
					return 0;
				
				default:
					break;
			}
		}
	}
}
