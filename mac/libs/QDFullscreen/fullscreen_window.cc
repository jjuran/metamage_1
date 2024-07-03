/*
	fullscreen_window.cc
	--------------------
*/

#include "fullscreen_window.hh"

// Mac OS X
#ifdef __APPLE__
#include <Carbon/Carbon.h>
#endif

// Mac OS
#ifndef __LOWMEM__
#include <LowMem.h>
#endif
#ifndef __MACWINDOWS__
#include <MacWindows.h>
#endif
#ifndef __MENUS__
#include <Menus.h>
#endif


#if ! TARGET_API_MAC_CARBON

static WindowRef fullscreen_window;
static short saved_MBarHeight;

void open_fullscreen_window()
{
	if ( fullscreen_window )
	{
		return;
	}
	
	const Rect& screenBounds = qd.screenBits.bounds;
	
	const Rect* r = &screenBounds;
	
	Boolean   vis    = false;
	short     procid = plainDBox;
	WindowRef behind = (WindowRef) -1;
	Boolean   box    = false;  // i.e. a close box
	
	fullscreen_window = NewWindow( NULL, r, "\p", vis, procid, behind, box, 0 );
	
	if ( ! fullscreen_window )
	{
		return;
	}
	
	SetPortWindowPort( fullscreen_window );
	
	saved_MBarHeight = LMGetMBarHeight();
	
	LMSetMBarHeight( 0 );
	
	ShowWindow( fullscreen_window );
	
	RectRgn( fullscreen_window->visRgn, r );
	
	FillRect( r, &qd.black );
}

void close_fullscreen_window()
{
	if ( ! fullscreen_window )
	{
		return;
	}
	
	FillRect( &fullscreen_window->portRect, &qd.black );
	
	LMSetMBarHeight( saved_MBarHeight );
	
	DisposeWindow( fullscreen_window );
	
	fullscreen_window = NULL;
	
	DrawMenuBar();
	
	/*
		This is just to make sure that fullscreen_window is no longer set as
		the current port.  We call SetPortWindowPort( FrontWindow() ) below,
		but we can't rely on it setting thePort if there are no windows.
		(In Mac OS 9 at least, it ignores NULL arguments.)
	*/
	
	qd.thePort = LMGetWMgrPort();
}

#else

int dummy;

#endif
