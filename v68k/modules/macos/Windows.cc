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


GrafPtr WMgrPort    : 0x09DE;
Pattern DeskPattern : 0x0A3C;
short   MBarHeight  : 0x0BAA;

const short End = quickdraw::Region_end;


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
	
	RgnHandle clipRgn = WMgrPort->clipRgn;
	
	RectRgn( clipRgn, &bounds );
	
	RgnHandle corner = (RgnHandle) NewHandle( corner_size );
	
	subtract_corner( clipRgn, corner, topLeft,     0,            0             );
	subtract_corner( clipRgn, corner, topRight,    bounds.right, 0             );
	subtract_corner( clipRgn, corner, bottomLeft,  0,            bounds.bottom );
	subtract_corner( clipRgn, corner, bottomRight, bounds.right, bounds.bottom );
	
	DisposeHandle( (Handle) corner );
	
	const QDGlobals& qd = get_QDGlobals();
	
	DeskPattern = qd.gray;
	
	if ( PatHandle deskPat = GetPattern( deskPatID ) )
	{
		DeskPattern = **deskPat;
		
		ReleaseResource( (Handle) deskPat );
	}
	
	calculate_menu_bar_height();
	
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
