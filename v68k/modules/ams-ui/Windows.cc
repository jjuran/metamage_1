/*
	Windows.cc
	----------
*/

#include "Windows.hh"

// Mac OS
#ifndef __CONTROLS__
#include <Controls.h>
#endif
#ifndef __MACMEMORY__
#include <MacMemory.h>
#endif
#ifndef __MACWINDOWS__
#include <MacWindows.h>
#endif
#ifndef __RESOURCES__
#include <Resources.h>
#endif

// iota
#include "iota/swap.hh"

// ams-common
#include "callouts.hh"
#include "QDGlobals.hh"
#include "raster_lock.hh"

// ams-ui
#include "MBDF.hh"
#include "modal_updating.hh"
#include "StrUtils.hh"
#include "WDEF.hh"
#include "desktop.hh"


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

const short bezel_corner_diameter = 16;


static
RgnHandle QDGlobalToLocalRegion( GrafPtr port, RgnHandle rgn )
{
	const Rect& bounds = port->portBits.bounds;
	
	OffsetRgn( rgn, bounds.left, bounds.top );
	
	return rgn;
}

static
long call_WDEF( WindowPeek window, short message, long param )
{
	WindowDefProcPtr wdef = &WDEF_0;
	
	const short varCode = *(Byte*) &window->windowDefProc;
	
	Handle h = window->windowDefProc;
	
	if ( (UInt32) h & 0x00FFFFFF )
	{
		wdef = (WindowDefProcPtr) *h;
	}
	
	return wdef( varCode, (WindowPtr) window, message, param );
}

static
Boolean insert_into_window_list( WindowPeek window, GrafPtr behind )
{
	const WindowPtr all = GrafPtr( -1 );  // Place all windows behind this one.
	
	if ( behind == all  ||  ! WindowList )
	{
		window->nextWindow = WindowList;
		
		WindowList = window;
		
		return -true;
	}
	
	if ( behind == (GrafPtr) WindowList )
	{
		window->nextWindow = WindowList->nextWindow;
		WindowList->nextWindow = window;
		
		return false;
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
	
	for ( ;  w != window;  w = w->nextWindow )
	{
		prev = w;
	}
	
	if ( w != NULL )
	{
		prev->nextWindow = window->nextWindow;
	}
}

#pragma mark -
#pragma mark Initialization and Allocation
#pragma mark -

pascal void InitWindows_patch()
{
	WindowList = NULL;
	
	WMgrPort = (GrafPtr) NewPtr( sizeof (GrafPort) );
	
	OpenPort( WMgrPort );
	
	const Rect& bounds = WMgrPort->portBits.bounds;
	
	PaintRect( &bounds );
	
	BezelRgn = NewRgn();
	
	OpenRgn();
	FrameRoundRect( &bounds, bezel_corner_diameter, bezel_corner_diameter );
	CloseRgn( BezelRgn );
	
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

pascal void GetWMgrPort_patch( struct GrafPort** port )
{
	*port = WMgrPort;
}

pascal struct GrafPort* NewWindow_patch( void*                 storage,
                                         const struct Rect*    bounds,
                                         const unsigned char*  title,
                                         Boolean               visible,
                                         short                 procID,
                                         struct GrafPort*      behind,
                                         unsigned char         closeBox,
                                         long                  refCon )
{
	WindowPeek window = (WindowPeek) storage;
	
	if ( window == NULL )
	{
		window = (WindowPeek) NewPtr( sizeof (WindowRecord) );
		
		if ( window == NULL )
		{
			return NULL;
		}
	}
	
	fast_memset( window, '\0', sizeof (WindowRecord) );
	
	GrafPtr& thePort   = *get_addrof_thePort();
	GrafPtr  savedPort = thePort;
	
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
	
	window->windowKind = userKind;
	window->visible    = -(visible != 0);
	window->hilited    = -true;
	window->goAwayFlag = closeBox;
	window->refCon     = refCon;
	
	window->windowDefProc = GetResource( 'WDEF', procID >> 4 );
	
	const short varCode = procID & 0x0F;
	
	*(Byte*) &window->windowDefProc = varCode;
	
	const Boolean frontmost = insert_into_window_list( window, behind );
	
	if ( ! frontmost )
	{
		thePort = savedPort;
	}
	
	window->hilited = frontmost;
	
	if ( ! visible )
	{
		SetEmptyRgn( port->visRgn );
		
		return (WindowPtr) window;
	}
	
	if ( frontmost )
	{
		if ( window->nextWindow )
		{
			if ( CurActivate != (WindowRef) window->nextWindow )
			{
				CurDeactive = (WindowRef) window->nextWindow;
			}
			
			HiliteWindow_patch( window->nextWindow, false );
		}
		
		CurActivate = (WindowRef) window;
	}
	
	call_WDEF( window, wCalcRgns, 0 );
	
	CalcVBehind_patch( window, window->strucRgn );
	PaintOne_patch   ( window, window->strucRgn );
	
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
	
	thePort = savedPort;
	
	if ( ! storage )
	{
		DisposePtr( (Ptr) window );
	}
	
	return NULL;
}

struct WIND_resource
{
	Rect    bounds;
	short   procID;
	short   visible;
	short   goAwayFlag;
	long    refCon;
	Str255  title;
};

pascal GrafPort* GetNewWindow_patch( short      resID,
                                     void*      storage,
                                     GrafPort*  behind )
{
	Handle h = GetResource( 'WIND', resID );
	
	if ( h == NULL )
	{
		return NULL;
	}
	
//	HLock( h );
	
	const WIND_resource& wind = *(const WIND_resource*) *h;
	
	WindowRef window = NewWindow( storage,
	                              &wind.bounds,
	                              wind.title,
	                              wind.visible,
	                              wind.procID,
	                              behind,
	                              wind.goAwayFlag,
	                              wind.refCon );
	
	ReleaseResource( h );
	
	return window;
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
	
	KillControls( port );
	
	WindowPeek window = (WindowPeek) port;
	
	const bool frontmost = window == WindowList;
	
	remove_from_window_list( window );
	
	if ( window->visible )
	{
		PaintBehind_patch( window->nextWindow, window->strucRgn );
		CalcVBehind_patch( window->nextWindow, window->strucRgn );
		
		if ( frontmost  &&  WindowList != NULL )
		{
			CurActivate = (WindowRef) WindowList;
			
			HiliteWindow_patch( WindowList, true );
		}
	}
	
	DisposeRgn( window->strucRgn  );
	DisposeRgn( window->contRgn   );
	DisposeRgn( window->updateRgn );
	
	if ( window->titleHandle )
	{
		DisposeHandle( (Handle) window->titleHandle );
	}
	
	ClosePort( port );
	
	GrafPtr thePort = *get_addrof_thePort();
	
	if ( thePort == port )
	{
		SetPort( WindowList ? (WindowRef) WindowList : WMgrPort );
	}
}

pascal void DisposeWindow_patch( struct GrafPort* window )
{
	CloseWindow( window );
	
	DisposePtr( (Ptr) window );
}

#pragma mark -
#pragma mark Window Display
#pragma mark -

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
		
		fast_memcpy( s, title, size );
	}
}

pascal void SelectWindow_patch( WindowPeek window )
{
	if ( window == WindowList )
	{
		return;
	}
	
	/*
		Guard against calling SelectWindow() twice in a row for different
		windows:  Don't give the front window a deactivate event if an
		activate event is pending for it, since (a) it's already in an
		inactive state, and (b) we'd be clobbering the deactivate event for
		the previous front window (if that too were still pending).
	*/
	
	if ( CurActivate != (WindowRef) WindowList )
	{
		CurDeactive = (WindowRef) WindowList;
	}
	
	CurActivate = (WindowRef) window;
	
	WindowPeek leader = WindowList;
	
	BringToFront_patch( window );
	HiliteWindow_patch( leader, false );
	HiliteWindow_patch( window, true  );
}

pascal void HideWindow_patch( WindowPeek window )
{
	if ( ! window->visible )
	{
		return;
	}
	
	ShowHide_patch( window, false );
	
	if ( window == WindowList  &&  window->nextWindow != NULL )
	{
		// TODO:  What if the next window is invisible?
		
		SelectWindow_patch( window->nextWindow );
	}
}

pascal void ShowWindow_patch( WindowPeek window )
{
	if ( window->visible )
	{
		return;
	}
	
	ShowHide_patch( window, true );
	
	// TODO:  Highlight and activate previously invisible front window
}

typedef pascal void (*window_painter)( WindowPeek, RgnHandle );

pascal void ShowHide_patch( WindowRecord* window, Boolean showFlag )
{
	if ( window == NULL  ||  ! window->visible == ! showFlag )
	{
		return;
	}
	
	window_painter paint_one_or_many = &PaintBehind_patch;
	
	window->visible = showFlag;
	
	if ( showFlag )
	{
		call_WDEF( window, wCalcRgns, 0 );
		
		paint_one_or_many = &PaintOne_patch;
	}
	
	CalcVBehind_patch( window, window->strucRgn );
	paint_one_or_many( window, window->strucRgn );
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

pascal void BringToFront_patch( WindowPeek window )
{
	if ( window == WindowList )
	{
		return;
	}
	
	WindowPeek w = WindowList;
	
	do
	{
		if ( w == NULL )
		{
			return;
		}
		
		w = w->nextWindow;
	}
	while ( w != window );
	
	// Calculate the obscured region of the window (including its structure).
	
	RectRgn( WMgrPort->clipRgn, &window->strucRgn[0]->rgnBBox );
	
	ClipAbove_patch( window );
	
	DiffRgn( window->strucRgn, WMgrPort->clipRgn, WMgrPort->clipRgn );
	
	remove_from_window_list( window );
	insert_into_window_list( window, GrafPtr( -1 ) );
	
	// Repaint only the previously hidden area.
	
	Rect r = WMgrPort->clipRgn[0]->rgnBBox;
	
	PaintOne_patch( window, WMgrPort->clipRgn );
	
	CalcVBehind_patch( window, window->strucRgn );
}

pascal void SendBehind_patch( WindowPeek window, WindowPeek behindWindow )
{
	if ( behindWindow != NULL )
	{
		if ( window == behindWindow  ||  window == behindWindow->nextWindow )
		{
			// SendBehind itself, or the one it's already behind.  Do nothing.
			return;
		}
	}
	else if ( window->nextWindow == NULL )
	{
		// SendBehind all, but window is already last.  Do nothing.
		return;
	}
	
	const bool was_frontmost = window == WindowList;
	
	WindowPeek w = WindowList;
	
	while ( w != window )
	{
		if ( w == NULL )
		{
			// The window to SendBehind isn't in the list.  Bail out.
			return;
		}
		
		if ( w == behindWindow )
		{
			// Sending forward -- just do it, let the caller update the visRgn.
			
			remove_from_window_list( window );
			insert_into_window_list( window, (WindowRef) behindWindow );
			
			return;
		}
		
		w = w->nextWindow;
	}
	
	// Found the window.  Remove and reinsert it.
	
	w = w->nextWindow;
	
	remove_from_window_list( window );
	insert_into_window_list( window, (WindowRef) behindWindow );
	
	// Repaint and recalc the windows in between the two z-order positions.
	
	while ( w != window )
	{
		PaintOne_patch( w, w->strucRgn );
		CalcVis_patch( w );
		
		w = w->nextWindow;
	}
	
	// Also recalc the lowered window.
	
	CalcVis_patch( w );
	
	if ( ! was_frontmost )
	{
		return;
	}
	
	if ( CurActivate != (WindowRef) window )
	{
		CurDeactive = (WindowRef) window;
	}
	
	WindowPeek leader = WindowList;
	
	CurActivate = (WindowRef) leader;
	
	HiliteWindow_patch( window, false );
	HiliteWindow_patch( leader, true  );
}

pascal WindowRef FrontWindow_patch()
{
	return (WindowRef) WindowList;
}

pascal void DrawGrowIcon_patch( WindowPeek window )
{
	QDGlobals& qd = get_QDGlobals();
	
	GrafPtr saved_port = qd.thePort;
	
	GrafPort* const port = &window->port;
	
	qd.thePort = port;
	
	call_WDEF( window, wDrawGIcon, 0 );
	
	qd.thePort = saved_port;
}

#pragma mark -
#pragma mark Mouse Location
#pragma mark -

pascal short FindWindow_patch( Point pt, WindowPtr* window )
{
	*window = NULL;
	
	if ( pt.v < MBarHeight )
	{
		return inMenuBar;
	}
	
	for ( WindowPeek w = WindowList;  w != NULL;  w = w->nextWindow )
	{
		if ( ! w->visible )
		{
			continue;
		}
		
		WindowPtr ptr = (WindowPtr) w;
		
		if ( short hit = call_WDEF( w, wHit, *(long*) &pt ) )
		{
			*window = ptr;
			
			return hit + 2;
		}
	}
	
	return inDesk;
}

pascal unsigned char TrackGoAway_patch( WindowRef window, Point pt )
{
	RgnHandle mouseRgn = NewRgn();
	
	WindowPeek w = (WindowPeek) window;
	
	QDGlobals& qd = get_QDGlobals();
	
	GrafPtr saved_port = qd.thePort;
	
	qd.thePort = WMgrPort;
	
	SetClip( w->strucRgn );
	
	SectRgn( WMgrPort->clipRgn, GrayRgn, WMgrPort->clipRgn );
	
	bool was_inside = false;
	bool is_inside = true;
	
	while ( true )
	{
		short hit = call_WDEF( w, wHit, *(long*) &pt );
		
		is_inside = hit == wInGoAway;
		
		if ( is_inside != was_inside )
		{
			was_inside = is_inside;
			
			call_WDEF( w, wDraw, wInGoAway );
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
		call_WDEF( w, wDraw, wInGoAway );
	}
	
	qd.thePort = saved_port;
	
	DisposeRgn( mouseRgn );
	
	return is_inside;
}

#pragma mark -
#pragma mark Window Movement and Sizing
#pragma mark -

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
	
	// Recalc the visRgn of this window and those below it.
	
	XorRgn( uncovered, window->strucRgn, uncovered );
	
	CalcVBehind_patch( window, uncovered );
	
	XorRgn( uncovered, window->strucRgn, uncovered );
	
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
	
	if ( activate  &&  ! window->hilited )
	{
		SelectWindow_patch( window );
	}
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
	
	call_WDEF( w, wCalcRgns, 0 );
	
	XorRgn( OldStructure, w->strucRgn, OldStructure );
	XorRgn( OldContent,   w->contRgn,  OldContent   );
	
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
	CalcVBehind_patch( w, exposed );
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
	
	if ( delta == 0 )
	{
		// TODO:  Don't raise the window if Command is down
		SelectWindow( w );
	}
	else if ( delta != 0x80008000 )
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
	
	call_WDEF( window, wGrow, (long) &grow_rect );
	
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
			
			call_WDEF( window, wGrow, (long) &grow_rect );
			
			grow_rect.bottom += event.where.v - pt.v;
			grow_rect.right  += event.where.h - pt.h;
			
			call_WDEF( window, wGrow, (long) &grow_rect );
			
			pt = event.where;
		}
	}
	
	call_WDEF( window, wGrow, (long) &grow_rect );
	
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

#pragma mark -
#pragma mark Update Region Maintenance
#pragma mark -

/*
	csdx, csdy:  Coordinate system deltas
	
	These are deltas from the global coordinate system to the local
	coordinate system.  Add them to global points to get local, and
	subtract them from local points to get global.
	
	Note that for a local origin below and to the right of the global
	origin, these values will be negative -- for example, to get from
	local (0,0) to global (100,80), you must add positive numbers or
	subtract negative ones.
*/

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

pascal void ValidRect_patch( const Rect* rect )
{
	RgnHandle rgn = rectangular_utility_region();
	
	RectRgn( rgn, rect );
	
	ValidRgn( rgn );
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

pascal void BeginUpdate_patch( struct GrafPort* window )
{
	WindowPeek w = (WindowPeek) window;
	
	using iota::swap;
	
	swap( window->visRgn, SaveVisRgn );
	
	QDGlobalToLocalRegion( window, w->updateRgn );
	
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

#pragma mark -
#pragma mark Miscellaneous Routines
#pragma mark -

pascal void SetWRefCon_patch( WindowRecord* window, long data )
{
	window->refCon = data;
}

pascal long GetWRefCon_patch( WindowRecord* window )
{
	return window->refCon;
}

#pragma mark -
#pragma mark Low-Level Routines
#pragma mark -

static inline
bool window_needs_update( WindowPeek w )
{
	return ! EmptyRgn( w->updateRgn );
}

static inline
bool window_could_be_updated_now( WindowPeek w )
{
	return ! exclusive_modal_updating  ||  w->windowKind == dialogKind;
}

pascal unsigned char CheckUpdate_patch( EventRecord* event )
{
	for ( WindowPeek w = WindowList;  w != NULL;  w = w->nextWindow )
	{
		if ( window_needs_update( w )  &&  window_could_be_updated_now( w ) )
		{
			fast_memset( event, '\0', sizeof (EventRecord) );
			
			event->what    = updateEvt;
			event->message = (long) w;
			
			return true;
		}
	}
	
	return false;
}

pascal void ClipAbove_patch( WindowPeek window )
{
	RgnHandle clipRgn = WMgrPort->clipRgn;
	
	SectRgn( clipRgn, GrayRgn, clipRgn );
	
	for ( WindowPeek w = WindowList;  w != window;  w = w->nextWindow )
	{
		if ( w == NULL )
		{
			break;
		}
		
		if ( w->visible )
		{
			DiffRgn( clipRgn, w->strucRgn, clipRgn );
		}
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
	else if ( window->visible )
	{
		call_WDEF( window, wDraw, 0 );
		
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
	for ( WindowPeek w = window;  w != NULL;  w = w->nextWindow )
	{
		PaintOne_patch( w, clobbered_region );
	}
	
	SaveUpdate = true;
	PaintWhite = true;
	
	PaintOne_patch( NULL, clobbered_region );
}

pascal void CalcVis_patch( WindowPeek window )
{
	RgnHandle visRgn = window->port.visRgn;
	
	if ( ! window->visible )
	{
		SetEmptyRgn( visRgn );
		return;
	}
	
	SectRgn( window->contRgn, GrayRgn, visRgn );
	
	for ( WindowPeek w = WindowList;  w != window;  w = w->nextWindow )
	{
		if ( w == NULL )
		{
			return;  // Specified window doesn't exist.
		}
		
		if ( w->visible )
		{
			DiffRgn( visRgn, w->strucRgn, visRgn );
		}
	}
	
	QDGlobalToLocalRegion( &window->port, visRgn );
}

pascal void CalcVBehind_patch( WindowPeek window, RgnHandle rgn )
{
	WindowPeek w = WindowList;
	
	for ( ;  w != window;  w = w->nextWindow )
	{
		if ( w == NULL )
		{
			return;
		}
	}
	
	for ( ;  w != NULL;  w = w->nextWindow )
	{
		CalcVis_patch( w );
	}
}
