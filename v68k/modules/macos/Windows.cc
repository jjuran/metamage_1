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

// quickdraw
#include "qd/region_detail.hh"

// macos
#include "Desk.hh"
#include "MBDF.hh"
#include "QDGlobals.hh"


GrafPtr   WMgrPort    : 0x09DE;
RgnHandle GrayRgn     : 0x09EE;
Pattern   DeskPattern : 0x0A3C;
short     MBarHeight  : 0x0BAA;

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


pascal void ClipAbove_patch( WindowPeek window )
{
	RgnHandle clipRgn = WMgrPort->clipRgn;
	
	SectRgn( clipRgn, GrayRgn, clipRgn );
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
	
	qd.thePort = saved_port;
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

pascal short FindWindow_patch( Point pt, WindowPtr* window )
{
	*window = NULL;
	
	if ( pt.v < MBarHeight )
	{
		return inMenuBar;
	}
	
	return inDesk;
}
