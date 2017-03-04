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

// iota
#include "iota/swap.hh"

// quickdraw
#include "qd/region_detail.hh"

// ams-core
#include "MBDF.hh"
#include "QDGlobals.hh"
#include "StrUtils.hh"
#include "WDEF.hh"
#include "desktop.hh"
#include "raster_lock.hh"


WindowPeek WindowList  : 0x09D6;
short      SaveUpdate  : 0x09DA;
short      PaintWhite  : 0x09DC;
GrafPtr    WMgrPort    : 0x09DE;
RgnHandle  OldStructure: 0x09E6;
RgnHandle  OldContent  : 0x09EA;
RgnHandle  GrayRgn     : 0x09EE;
RgnHandle  SaveVisRgn  : 0x09F2;
Pattern    DeskPattern : 0x0A3C;
WindowRef  CurActivate : 0x0A64;
WindowRef  CurDeactive : 0x0A68;
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

pascal void SetWTitle_patch( WindowPeek window, const unsigned char* s )
{
	if ( s != NULL  &&  s[ 0 ] != 0 )
	{
		if ( window->titleHandle != NULL )
		{
			SetString_patch( window->titleHandle, s );
		}
		else
		{
			window->titleHandle = NewString_patch( s );
		}
		
		window->titleWidth = StringWidth( s );
	}
	else
	{
		if ( window->titleHandle != NULL )
		{
			DisposeHandle( (Handle) window->titleHandle );
			
			window->titleHandle = NULL;
		}
		
		window->titleWidth = 0;
	}
}

pascal void GetWTitle_patch( WindowPeek window, unsigned char* s )
{
	s[ 0 ] = '\0';
	
	if ( window->titleHandle )
	{
		const unsigned char* title = *window->titleHandle;
		
		const Size size = 1 + title[ 0 ];
		
		memcpy( s, title, size );
	}
}

pascal void DrawGrowIcon_patch( WindowPeek window )
{
	QDGlobals& qd = get_QDGlobals();
	
	GrafPtr saved_port = qd.thePort;
	
	GrafPort* const port = &window->port;
	
	qd.thePort = port;
	
	const short varCode = *(Byte*) &window->windowDefProc;
	
	WDEF_0( varCode, port, wDrawGIcon, 0 );
	
	qd.thePort = saved_port;
}

pascal void ClipAbove_patch( WindowPeek window )
{
	RgnHandle clipRgn = WMgrPort->clipRgn;
	
	SectRgn( clipRgn, GrayRgn, clipRgn );
	
	WindowPeek w = WindowList;
	
	while ( w != window )
	{
		if ( w == NULL )
		{
			break;
		}
		
		DiffRgn( clipRgn, w->strucRgn, clipRgn );
		
		w = w->nextWindow;
	}
}

pascal void PaintOne_patch( WindowPeek window, RgnHandle clobbered_region )
{
	if ( clobbered_region == NULL  ||  EmptyRgn( clobbered_region ) )
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
		
		if ( SaveUpdate )
		{
			UnionRgn( window->updateRgn, clobbered_region, window->updateRgn );
		}
		
		if ( PaintWhite )
		{
			EraseRect( &window->contRgn[0]->rgnBBox );
		}
	}
	
	qd.thePort = saved_port;
}

pascal void PaintBehind_patch( WindowPeek window, RgnHandle clobbered_region )
{
	WindowPeek w = window;
	
	while ( w != NULL )
	{
		PaintOne_patch( w, clobbered_region );
		
		w = w->nextWindow;
	}
	
	SaveUpdate = true;
	PaintWhite = true;
	
	PaintOne_patch( NULL, clobbered_region );
}

static
bool window_needs_update( WindowPeek w )
{
	if ( EmptyRgn( w->updateRgn ) )
	{
		return false;
	}
	
	// Clip the update region to the visRgn and check again.
	
	SectRgn( w->port.visRgn, w->updateRgn, w->updateRgn );
	
	return ! EmptyRgn( w->updateRgn );
}

pascal unsigned char CheckUpdate_patch( EventRecord* event )
{
	WindowPeek w = WindowList;
	
	while ( w != NULL )
	{
		if ( window_needs_update( w ) )
		{
			memset( event, '\0', sizeof (EventRecord) );
			
			event->what    = updateEvt;
			event->message = (long) w;
			
			return true;
		}
		
		w = w->nextWindow;
	}
	
	return false;
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
	SaveUpdate = true;
	PaintWhite = true;
	
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
	
	OldStructure = NewRgn();
	OldContent   = NewRgn();
	
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
	
	SaveVisRgn = NewRgn();
	
	CurActivate = NULL;
	CurDeactive = NULL;
	
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
	
	WindowPeek prev = NULL;
	WindowPeek next = WindowList;
	
	do
	{
		prev = next;
		
		next = next->nextWindow;
	}
	while ( next != NULL  &&  (GrafPtr) next != behind );
	
	window->nextWindow = next;
	prev->nextWindow = window;
	
	return false;
}

static
void remove_from_window_list( WindowPeek window )
{
	if ( window == WindowList )
	{
		WindowList = window->nextWindow;
		
		return;
	}
	
	WindowPeek prev;
	WindowPeek w = WindowList;
	
	while ( w != window )
	{
		prev = w;
		
		w = w->nextWindow;
	}
	
	if ( w != NULL )
	{
		prev->nextWindow = window->nextWindow;
	}
}

pascal struct GrafPort* NewWindow_patch( void*                 storage,
                                         const struct Rect*    bounds,
                                         const unsigned char*  title,
                                         short                 visible,
                                         short                 procID,
                                         struct GrafPort*      behind,
                                         unsigned char         closeBox,
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
	
	window->strucRgn  = NewRgn();  if ( window->strucRgn  == NULL )  goto fail_1;
	window->contRgn   = NewRgn();  if ( window->contRgn   == NULL )  goto fail_2;
	window->updateRgn = NewRgn();  if ( window->updateRgn == NULL )  goto fail_3;
	
	SetWTitle_patch( window, title );
	
	if ( title != NULL  &&  title[ 0 ] != 0  &&  window->titleHandle == NULL )
	{
		goto fail_4;
	}
	
	PortSize( bounds->right - bounds->left, bounds->bottom - bounds->top );
	
	MovePortTo( bounds->left, bounds->top );
	
	if ( visible )
	{
		RectRgn( port->visRgn, bounds );
		
		CurActivate = (WindowRef) window;
	}
	else
	{
		SetEmptyRgn( port->visRgn );
	}
	
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
	
fail_4:
	
	DisposeRgn( window->updateRgn );
	
fail_3:
	
	DisposeRgn( window->contRgn );
	
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

pascal void MoveWindow_patch( WindowRef w, short h, short v, char activate )
{
	WindowPeek window = (WindowPeek) w;
	
	const short top  = -w->portBits.bounds.top;
	const short left = -w->portBits.bounds.left;
	
	const short dh = h - left;
	const short dv = v - top;
	
	QDGlobals& qd = get_QDGlobals();
	
	GrafPtr saved_port = qd.thePort;
	
	qd.thePort = w;
	
	MovePortTo( h, v );
	
	qd.thePort = saved_port;
	
	/*
		Theory:
		
		Moving a visible window results in three affected regions that require
		screen operations.  The first is the surface of the window that can be
		blitted directly.  The second contains the newly exposed area of the
		window.  The third consists of uncovered parts of other windows and
		the desktop.
	*/
	
	RgnHandle uncovered = NULL;
	
	if ( window->visible )
	{
		qd.thePort = WMgrPort;
		
		// Clip to the gray region minus any nearer windows' structure.
		
		SetClip( GrayRgn );
		
		ClipAbove( w );
		
		// Set the uncovered region to the visible portion of the window.
		
		uncovered = NewRgn();
		
		SectRgn( window->strucRgn, WMgrPort->clipRgn, uncovered );
	}
	
	// Use the structure region's bounding box as the srcRect for blitting.
	
	const Rect srcRect = window->strucRgn[0]->rgnBBox;
	
	// Offset the window's static regions, even if it's invisible.
	
	OffsetRgn( window->strucRgn, dh, dv );
	OffsetRgn( window->contRgn,  dh, dv );
	
	if ( ! window->visible )
	{
		return;
	}
	
	// For visible windows, also offset the update region.
	
	OffsetRgn( window->updateRgn, dh, dv );
	
	/*
		Set the visRgn of the window's port.  We don't have CalcVis() yet,
		but this works for a single window.
	*/
	
	SectRgn( window->contRgn, GrayRgn, w->visRgn );
	
	/*
		Further clip to the visible part of the old structure (translated).
		This means that only visible window bits will be blitted, skipping
		anything covered by the menu bar or screen corners (or in the future,
		other windows).
	*/
	
	OffsetRgn( uncovered, dh, dv );
	
	SectRgn( uncovered, WMgrPort->clipRgn, WMgrPort->clipRgn );
	
	OffsetRgn( uncovered, -dh, -dv );
	
	/*
		Remove the new structure region from the uncovered region.  Anything
		under the new location of the window is obviously not uncovered.
	*/
	
	DiffRgn( uncovered, window->strucRgn, uncovered );
	
	const Rect dstRect = window->strucRgn[0]->rgnBBox;
	
	/*
		Blit the window.  Make sure to use WMgrPort->portBits, not screenBits,
		as the destination bitmap, so that WMgrPort->clipRgn will be applied
		(and so we don't waste time creating a temporary port).
	*/
	
	CopyBits( &WMgrPort->portBits,
	          &WMgrPort->portBits,
	          &srcRect,
	          &dstRect,
	          srcCopy,
	          NULL );
	
	/*
		The Window Manager port's clip region excludes parts of the window
		structure that couldn't be blitted because they were only now exposed
		and weren't visible on the screen.  Subtract the remaining, blittable
		area from the structure region to get the exposed region which now
		must be redrawn.  (Continue to use WMgrPort->clipRgn for storage.)
		Paint the window frame, and erase any exposed content area and add it
		to the update region.
	*/
	
	DiffRgn( window->strucRgn, WMgrPort->clipRgn, WMgrPort->clipRgn );
	
	PaintOne_patch( window, WMgrPort->clipRgn );
	
	// Repaint areas of the desktop or other windows that have become visible.
	
	PaintBehind_patch( window->nextWindow, uncovered );
	
	qd.thePort = saved_port;
	
	DisposeRgn( uncovered );
}

pascal void SizeWindow_patch( WindowRef window, short h, short v, char update )
{
	WindowPeek w = (WindowPeek) window;
	
	CopyRgn( w->strucRgn, OldStructure );
	CopyRgn( w->contRgn,  OldContent   );
	
	const Rect& portRect = window->portRect;
	
	const short old_height = portRect.bottom - portRect.top;
	const short old_width  = portRect.right - portRect.left;
	
	QDGlobals& qd = get_QDGlobals();
	
	GrafPtr saved_port = qd.thePort;
	
	qd.thePort = window;
	
	PortSize( h, v );
	
	qd.thePort = saved_port;
	
	const short varCode = *(Byte*) &w->windowDefProc;
	
	WDEF_0( varCode, window, wCalcRgns, 0 );
	
	XorRgn( OldStructure, w->strucRgn, OldStructure );
	XorRgn( OldContent,   w->contRgn,  OldContent   );
	
	if ( w->visible )
	{
		SectRgn( w->contRgn, GrayRgn, window->visRgn );
	}
	
	RgnHandle exposed = OldStructure;
	
	UnionRgn( OldStructure, OldContent, exposed );
	
	/*
		Add the newly-sized window frame to the exposed region.
		We have to do this because resizing the window invalidates at least
		the window title, and potentially more -- and that's up to the WDEF,
		so to be conservative, consider the entire window frame exposed.
	*/
	
	RgnHandle frame = OldContent;
	
	XorRgn( w->strucRgn, w->contRgn, frame );
	
	UnionRgn( exposed, frame, exposed );
	
	SaveUpdate = update;
	
	PaintBehind_patch( w, exposed );
}

pascal void HiliteWindow_patch( WindowPeek window, unsigned char hilite )
{
	if ( window->hilited == hilite )
	{
		return;
	}
	
	window->hilited = hilite;
	
	SaveUpdate = false;
	PaintWhite = false;
	
	PaintOne_patch( window, window->strucRgn );
	
	SaveUpdate = true;
	PaintWhite = true;
}

pascal unsigned char TrackGoAway_patch( WindowRef window, Point pt )
{
	RgnHandle mouseRgn = NewRgn();
	
	WindowPeek w = (WindowPeek) window;
	
	const short varCode = *(Byte*) &w->windowDefProc;
	
	QDGlobals& qd = get_QDGlobals();
	
	GrafPtr saved_port = qd.thePort;
	
	qd.thePort = WMgrPort;
	
	SetClip( w->strucRgn );
	
	bool was_inside = false;
	bool is_inside = true;
	
	while ( true )
	{
		short hit = WDEF_0( varCode, window, wHit, *(long*) &pt );
		
		is_inside = hit == wInGoAway;
		
		if ( is_inside != was_inside )
		{
			was_inside = is_inside;
			
			WDEF_0( varCode, window, wDraw, wInGoAway );
		}
		
		SetRectRgn( mouseRgn, pt.h, pt.v, pt.h + 1, pt.v + 1 );
		
		EventRecord event;
		
		if ( WaitNextEvent( mUpMask, &event, 0x7fffffff, mouseRgn ) )
		{
			if ( event.what == mouseUp )
			{
				break;
			}
		}
		
		pt = event.where;
	}
	
	if ( is_inside )
	{
		WDEF_0( varCode, window, wDraw, wInGoAway );
	}
	
	qd.thePort = saved_port;
	
	DisposeRgn( mouseRgn );
	
	return is_inside;
}

pascal void BeginUpdate_patch( struct GrafPort* window )
{
	WindowPeek w = (WindowPeek) window;
	
	using iota::swap;
	
	swap( window->visRgn, SaveVisRgn );
	
	SectRgn( SaveVisRgn, w->updateRgn, window->visRgn );
	
	SetEmptyRgn( w->updateRgn );
}

pascal void EndUpdate_patch( struct GrafPort* window )
{
	WindowPeek w = (WindowPeek) window;
	
	using iota::swap;
	
	swap( window->visRgn, SaveVisRgn );
	
	SetEmptyRgn( SaveVisRgn );
}

pascal WindowRef FrontWindow_patch()
{
	return (WindowRef) WindowList;
}

pascal void DragWindow_patch( WindowRef w, Point start, const Rect* bounds )
{
	WindowPeek window = (WindowPeek) w;
	
	RgnHandle drag_region = NewRgn();
	
	CopyRgn( window->strucRgn, drag_region );
	
	QDGlobals& qd = get_QDGlobals();
	
	GrafPtr saved_port = qd.thePort;
	
	qd.thePort = WMgrPort;
	
	SetClip( GrayRgn );
	
	long delta = DragGrayRgn( drag_region, start, bounds, bounds, 0, NULL );
	
	qd.thePort = saved_port;
	
	if ( delta != 0  &&  delta != 0x80008000 )
	{
		const short dh = delta;
		const short dv = delta >> 16;
	
		short v = -w->portBits.bounds.top;
		short h = -w->portBits.bounds.left;
	
		h += dh;
		v += dv;
	
		MoveWindow( w, h, v, true );
	}
	
	DisposeRgn( drag_region );
}

static
RgnHandle rectangular_utility_region()
{
	static RgnHandle rgn = NewRgn();
	
	return rgn;
}

pascal void InvalRect_patch( const Rect* rect )
{
	RgnHandle rgn = rectangular_utility_region();
	
	RectRgn( rgn, rect );
	
	InvalRgn( rgn );
}

pascal void ValidRect_patch( const Rect* rect )
{
	RgnHandle rgn = rectangular_utility_region();
	
	RectRgn( rgn, rect );
	
	ValidRgn( rgn );
}

pascal void InvalRgn_patch( struct MacRegion** rgn )
{
	GrafPtr thePort = *get_addrof_thePort();
	
	WindowPeek w = (WindowPeek) thePort;
	
	const short csdx = thePort->portBits.bounds.left;
	const short csdy = thePort->portBits.bounds.top;
	
	OffsetRgn( rgn, -csdx, -csdy );  // local to global
	
	UnionRgn( w->updateRgn, rgn, w->updateRgn );
	
	OffsetRgn( rgn, csdx, csdy );  // global to local
}

pascal void ValidRgn_patch( struct MacRegion** rgn )
{
	GrafPtr thePort = *get_addrof_thePort();
	
	WindowPeek w = (WindowPeek) thePort;
	
	const short csdx = thePort->portBits.bounds.left;
	const short csdy = thePort->portBits.bounds.top;
	
	OffsetRgn( rgn, -csdx, -csdy );  // local to global
	
	DiffRgn( w->updateRgn, rgn, w->updateRgn );
	
	OffsetRgn( rgn, csdx, csdy );  // global to local
}

pascal long GrowWindow_patch( WindowRef w, Point start, const Rect* size )
{
	WindowPeek window = (WindowPeek) w;
	
	RgnHandle mouseRgn = NewRgn();
	
	QDGlobals& qd = get_QDGlobals();
	
	GrafPtr saved_port = qd.thePort;
	
	qd.thePort = WMgrPort;
	
	SetClip( GrayRgn );
	
	PenMode( notPatXor );
	PenPat( &qd.gray );
	
	Rect grow_rect = window->contRgn[0]->rgnBBox;
	
	Point pt = start;
	
	SetRectRgn( mouseRgn, pt.h, pt.v, pt.h + 1, pt.v + 1 );
	
	const short varCode = *(Byte*) &window->windowDefProc;
	
	WDEF_0( varCode, w, wGrow, (long) &grow_rect );
	
	long sleep = 0;
	
	while ( true )
	{
		EventRecord event;
		
		const bool eventful = WaitNextEvent( mUpMask, &event, sleep, mouseRgn );
		
		// Events for DAs won't occur here, so we don't need to check.
		
		if ( event.what == mouseUp )
		{
			break;
		}
		
		const Point& where = event.where;
		
		SetRectRgn( mouseRgn, where.h, where.v, where.h + 1, where.v + 1 );
		
		if ( event.what != nullEvent )
		{
			/*
				Discard any mouse-moved events without redrawing the region.
				This can be prohibitively CPU-taxing on lightweight systems.
				
				Set sleep to 0 to ensure that we get a null event afterward.
			*/
			
			sleep = 0;
			
			continue;
		}
		
		/*
			We got a null event -- redraw the dotted grow region (if it
			actually moved, that is), and set sleep back to forever (approx).
		*/
		
		sleep = 0x7fffffff;
		
		if ( *(long*) &pt != *(long*) &event.where )
		{
			raster_lock lock;
			
			WDEF_0( varCode, w, wGrow, (long) &grow_rect );
			
			grow_rect.bottom += event.where.v - pt.v;
			grow_rect.right  += event.where.h - pt.h;
			
			WDEF_0( varCode, w, wGrow, (long) &grow_rect );
			
			pt = event.where;
		}
	}
	
	WDEF_0( varCode, w, wGrow, (long) &grow_rect );
	
	PenNormal();
	
	qd.thePort = saved_port;
	
	DisposeRgn( mouseRgn );
	
	if ( *(long*) &pt == *(long*) &start )
	{
		return 0;
	}
	
	Point new_size =
	{
		grow_rect.bottom - grow_rect.top,
		grow_rect.right - grow_rect.left,
	};
	
	return *(long*) &new_size;
}

pascal short FindWindow_patch( Point pt, WindowPtr* window )
{
	*window = NULL;
	
	if ( pt.v < MBarHeight )
	{
		return inMenuBar;
	}
	
	WindowPeek w = WindowList;
	
	while ( w != NULL )
	{
		const short varCode = *(Byte*) &w->windowDefProc;
		
		WindowPtr ptr = (WindowPtr) w;
		
		if ( short hit = WDEF_0( varCode, ptr, wHit, *(long*) &pt ) )
		{
			*window = ptr;
			
			return hit + 2;
		}
		
		w = w->nextWindow;
	}
	
	return inDesk;
}

pascal void CloseWindow_patch( struct GrafPort* port )
{
	if ( CurActivate == port )
	{
		CurActivate = NULL;
	}
	
	if ( CurDeactive == port )
	{
		CurDeactive = NULL;
	}
	
	WindowPeek window = (WindowPeek) port;
	
	const bool frontmost = window == WindowList;
	
	remove_from_window_list( window );
	
	PaintBehind( (WindowRef) window->nextWindow, window->strucRgn );
	
	if ( frontmost  &&  WindowList != NULL )
	{
		CurActivate = (WindowRef) WindowList;
		
		HiliteWindow_patch( WindowList, true );
	}
	
	DisposeRgn( window->strucRgn  );
	DisposeRgn( window->contRgn   );
	DisposeRgn( window->updateRgn );
	
	if ( window->titleHandle )
	{
		DisposeHandle( (Handle) window->titleHandle );
	}
	
	ClosePort( port );
}
