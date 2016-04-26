/*
	Windows.cc
	----------
*/

#include "Windows.hh"

// Mac OS
#ifndef __MACMEMORY__
#include <MacMemory.h>
#endif
#ifndef __MACWINDOWS__
#include <MacWindows.h>
#endif
#ifndef __RESOURCES__
#include <Resources.h>
#endif

// Standard C
#include <string.h>

// quickdraw
#include "qd/region_detail.hh"

// macos
#include "Desk.hh"
#include "MBDF.hh"
#include "QDGlobals.hh"
#include "WDEF.hh"


WindowPeek WindowList  : 0x09D6;
GrafPtr    WMgrPort    : 0x09DE;
RgnHandle  GrayRgn     : 0x09EE;
Pattern    DeskPattern : 0x0A3C;
short      MBarHeight  : 0x0BAA;

/*
	BezelRgn is a made-up global (i.e. not a Mac OS low memory global).
	It includes the entire screen minus the rounded corners.
	
	Rationale:
	 * It's suitable as a clip region for the Window Manager port while
	   drawing the menu bar.
	 * It can be used to calculate GrayRgn (by subtracting the menu bar rect)
	   when the menu bar height changes (which includes hiding and showing
	   the menu bar).
*/

RgnHandle BezelRgn;

const short End = quickdraw::Region_end;


static
void draw_window( short varCode, const WindowRecord& window )
{
	QDGlobals& qd = get_QDGlobals();
	
	GrafPtr saved_port = qd.thePort;
	
	qd.thePort = WMgrPort;
	
	WDEF_0( varCode, (WindowPtr) &window, wDraw, 0 );
	
	qd.thePort = saved_port;
}

static
void draw_window( WindowPeek window )
{
	draw_window( *(Byte*) &window->windowDefProc, *window );
}

pascal void ClipAbove_patch( WindowPeek window )
{
	RgnHandle clipRgn = WMgrPort->clipRgn;
	
	SectRgn( clipRgn, GrayRgn, clipRgn );
	
	WindowPeek w = WindowList;
	
	if ( w != window )
	{
		DiffRgn( clipRgn, w->strucRgn, clipRgn );
	}
}

pascal void PaintOne_patch( WindowPeek window, RgnHandle clobbered_region )
{
	if ( clobbered_region == NULL )
	{
		return;
	}
	
	QDGlobals& qd = get_QDGlobals();
	
	GrafPtr saved_port = qd.thePort;
	
	qd.thePort = WMgrPort;
	
	SetClip( clobbered_region );
	
	ClipAbove_patch( window );
	
	if ( window == NULL )
	{
		draw_desktop_from_WMgrPort();
	}
	else
	{
		const short varCode = *(Byte*) &window->windowDefProc;
		
		WDEF_0( varCode, (WindowPtr) window, wDraw, 0 );
		
		EraseRect( &window->contRgn[0]->rgnBBox );
	}
	
	qd.thePort = saved_port;
}

pascal void PaintBehind_patch( WindowPeek window, RgnHandle clobbered_region )
{
	WindowPeek w = window;
	
	if ( w != NULL )
	{
		PaintOne_patch( w, clobbered_region );
	}
	
	PaintOne_patch( NULL, clobbered_region );
}


static const short corner_size = (1 + 4 + 4 * 5 + 1) * sizeof (short);  // 52

static const short topLeft[] =
{
	corner_size,
	0, 0, 5, 5,
	
	0,  0,             5,  End,
	1,           3,    5,  End,
	2,        2, 3,        End,
	3,     1, 2,           End,
	//
	5,  0, 1,              End,
	
	End
};

static const short topRight[] =
{
	corner_size,
	0, -5, 5, 0,
	
	0,  -5,                 0,  End,
	1,  -5,     -3,             End,
	2,          -3, -2,         End,
	3,              -2, -1,     End,
	//
	5,                  -1, 0,  End,
	
	End
};

static const short bottomLeft[] =
{
	corner_size,
	-5, 0, 0, 5,
	
	-5,  0, 1,              End,
	//
	-3,     1, 2,           End,
	-2,        2, 3,        End,
	-1,           3,    5,  End,
	-0,  0,             5,  End,
	
	End
};

static const short bottomRight[] =
{
	corner_size,
	-5, -5, 0, 0,
	
	-5,                  -1, 0,  End,
	//
	-3,              -2, -1,     End,
	-2,          -3, -2,         End,
	-1,  -5,     -3,             End,
	-0,  -5,                 0,  End,
	
	End
};

static void subtract_corner( RgnHandle     clipRgn,
                             RgnHandle     cornerRgn,
                             const short*  corner_data,
                             short         dh,
                             short         dv )
{
	BlockMoveData( corner_data, *cornerRgn, corner_size );
	
	if ( dh != 0  ||  dv != 0 )
	{
		OffsetRgn( cornerRgn, dh, dv );
	}
	
	XorRgn( clipRgn, cornerRgn, clipRgn );
}

pascal void InitWindows_patch()
{
	WindowList = NULL;
	
	WMgrPort = (GrafPtr) NewPtr( sizeof (GrafPort) );
	
	OpenPort( WMgrPort );
	
	const Rect& bounds = WMgrPort->portBits.bounds;
	
	PaintRect( &bounds );
	
	BezelRgn = NewRgn();
	
	RectRgn( BezelRgn, &bounds );
	
	RgnHandle corner = (RgnHandle) NewHandle( corner_size );
	
	subtract_corner( BezelRgn, corner, topLeft,     0,            0             );
	subtract_corner( BezelRgn, corner, topRight,    bounds.right, 0             );
	subtract_corner( BezelRgn, corner, bottomLeft,  0,            bounds.bottom );
	subtract_corner( BezelRgn, corner, bottomRight, bounds.right, bounds.bottom );
	
	DisposeHandle( (Handle) corner );
	
	SetClip( BezelRgn );
	
	const QDGlobals& qd = get_QDGlobals();
	
	DeskPattern = qd.gray;
	
	if ( PatHandle deskPat = GetPattern( deskPatID ) )
	{
		DeskPattern = **deskPat;
		
		ReleaseResource( (Handle) deskPat );
	}
	
	calculate_menu_bar_height();
	
	GrayRgn = NewRgn();
	
	Rect menubar = bounds;
	
	menubar.bottom = MBarHeight;
	
	RectRgn( GrayRgn, &menubar );
	
	DiffRgn( BezelRgn, GrayRgn, GrayRgn );
	
	draw_desktop_from_WMgrPort();
	draw_menu_bar_from_WMgr_port();
}

static
Boolean insert_into_window_list( WindowPeek window, GrafPtr behind )
{
	const WindowPtr all = GrafPtr( -1 );  // Place all windows behind this one.
	
	if ( behind == all  ||  behind == (GrafPtr) WindowList  ||  ! WindowList )
	{
		window->nextWindow = WindowList;
		
		WindowList = window;
		
		return -true;
	}
	
	return false;
}

static
void remove_from_window_list( WindowPeek window )
{
	if ( window == WindowList )
	{
		WindowList = window->nextWindow;
	}
}

pascal struct GrafPort* NewWindow_patch( void*                 storage,
                                         const struct Rect*    bounds,
                                         const unsigned char*  title,
                                         short                 visible,
                                         short                 procID,
                                         struct GrafPort*      behind,
                                         short                 closeBox,
                                         long                  refCon )
{
	if ( WindowList != NULL )
	{
		return NULL;  // Only one window allowed, for now.
	}
	
	WindowPeek window = (WindowPeek) storage;
	
	if ( window == NULL )
	{
		window = (WindowPeek) NewPtr( sizeof (WindowRecord) );
		
		if ( window == NULL )
		{
			return NULL;
		}
	}
	
	memset( window, '\0', sizeof (WindowRecord) );
	
	GrafPtr port = &window->port;
	
	OpenPort( port );
	
	if ( port->device < 0 )  goto fail_0;
	
	window->strucRgn = NewRgn();  if ( window->strucRgn == NULL )  goto fail_1;
	window->contRgn  = NewRgn();  if ( window->contRgn  == NULL )  goto fail_2;
	
	PortSize( bounds->right - bounds->left, bounds->bottom - bounds->top );
	
	MovePortTo( bounds->left, bounds->top );
	
	window->windowKind = userKind;
	window->visible    = -(visible != 0);
	window->hilited    = -true;
	window->goAwayFlag = closeBox;
	window->refCon     = refCon;
	
	const short varCode = procID & 0x0F;
	
	*(Byte*) &window->windowDefProc = varCode;
	
	const Boolean frontmost = insert_into_window_list( window, behind );
	
	window->hilited = frontmost;
	
	WDEF_0( varCode, (WindowPtr) window, wCalcRgns, 0 );
	
	PaintOne_patch( window, window->strucRgn );
	
	return (WindowPtr) window;
	
fail_2:
	
	DisposeRgn( window->strucRgn );
	
fail_1:
	
	ClosePort( (WindowPtr) window );
	
fail_0:
	
	if ( ! storage )
	{
		DisposePtr( (Ptr) window );
	}
	
	return NULL;
}

pascal void DisposeWindow_patch( struct GrafPort* window )
{
	CloseWindow( window );
	
	DisposePtr( (Ptr) window );
}

pascal WindowRef FrontWindow_patch()
{
	return (WindowRef) WindowList;
}

pascal short FindWindow_patch( Point pt, WindowPtr* window )
{
	*window = NULL;
	
	if ( pt.v < MBarHeight )
	{
		return inMenuBar;
	}
	
	WindowPeek w = WindowList;
	
	if ( w != NULL )
	{
		const short varCode = *(Byte*) &w->windowDefProc;
		
		WindowPtr ptr = (WindowPtr) w;
		
		if ( short hit = WDEF_0( varCode, ptr, wHit, *(long*) &pt ) )
		{
			*window = ptr;
			
			return hit + 2;
		}
	}
	
	return inDesk;
}

pascal void CloseWindow_patch( struct GrafPort* port )
{
	WindowPeek window = (WindowPeek) port;
	
	remove_from_window_list( window );
	
	PaintBehind( (WindowRef) window->nextWindow, window->strucRgn );
	
	DisposeRgn( window->strucRgn );
	DisposeRgn( window->contRgn  );
	
	ClosePort( port );
}
