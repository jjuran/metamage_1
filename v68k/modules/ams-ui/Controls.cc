/*
	Controls.cc
	-----------
*/

#include "Controls.hh"

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

// ams-common
#include "callouts.hh"
#include "raster_lock.hh"
#include "scoped_zone.hh"

// ams-ui
#include "CDEF.hh"
#include "scoped_port.hh"


static
long call_CDEF( ControlRef control, short message, long param )
{
	ControlDefProcPtr cdef = (ControlDefProcPtr) *control[0]->contrlDefProc;
	
	const short varCode = *(Byte*) &control[0]->contrlDefProc;
	
	return cdef( varCode, control, message, param );
}

static
void draw_control( ControlRef control, short part )
{
	scoped_port thePort = control[0]->contrlOwner;
	
	raster_lock lock;
	
	call_CDEF( control, drawCntl, part );
}

static
void draw_control( ControlRef control )
{
	draw_control( control, control[0]->contrlHilite );
}

#pragma mark -
#pragma mark Initialization and Allocation
#pragma mark -

static Ptr gDefProcPtr = (Ptr) &CDEF_0;

pascal ControlRecord** NewControl_patch( GrafPort*             window,
                                         const Rect*           bounds,
                                         const unsigned char*  title,
                                         Boolean               visible,
                                         short                 value,
                                         short                 min,
                                         short                 max,
                                         short                 procID,
                                         long                  refCon )
{
	ControlRef control = (ControlRef) NewHandleClear( sizeof (ControlRecord) );
	
	if ( control )
	{
		Handle cdef = GetResource( 'CDEF', procID >> 4 );
		
		WindowPeek w = (WindowPeek) window;
		
		control[0]->nextControl   = (ControlRef) w->controlList;
		control[0]->contrlOwner   = window;
		control[0]->contrlRect    = *bounds;
		control[0]->contrlVis     = -!! visible;
	//	control[0]->contrlHilite  = 0;
		control[0]->contrlValue   = value;
		control[0]->contrlMin     = min;
		control[0]->contrlMax     = max;
		control[0]->contrlDefProc = cdef ? cdef : &gDefProcPtr;
	//	control[0]->contrlData    = NULL;
	//	control[0]->contrlAction  = NULL;
		control[0]->contrlRfCon   = refCon;
		
		if ( title )
		{
			fast_memcpy( control[0]->contrlTitle, title, 1 + title[ 0 ] );
		}
		
		const short varCode = procID & 0x0F;
		
		*(Byte*) &control[0]->contrlDefProc = varCode;
		
		w->controlList = (Handle) control;
		
		if ( visible )
		{
			draw_control( control );
		}
	}
	
	return control;
}

struct CNTL_resource
{
	Rect    bounds;
	short   value;
	short   visible;
	short   max;
	short   min;
	short   procID;
	long    refCon;
	Str255  title;
};

pascal ControlRecord** GetNewControl_patch( short controlID, WindowRef window )
{
	Handle h = GetResource( 'CNTL', controlID );
	
	if ( h == NULL )
	{
		return NULL;
	}
	
	const CNTL_resource cntl = *(const CNTL_resource*) *h;
	
	ReleaseResource( h );
	
	ControlRef control = NewControl( window,
	                                 &cntl.bounds,
	                                 cntl.title,
	                                 (Boolean&) cntl.visible,
	                                 cntl.value,
	                                 cntl.min,
	                                 cntl.max,
	                                 cntl.procID,
	                                 cntl.refCon );
	
	return control;
}

pascal void DisposeControl_patch( ControlRecord** control )
{
	HideControl( control );
	
	WindowPeek w = (WindowPeek) control[0]->contrlOwner;
	
	ControlRef* slot = (ControlRef*) &w->controlList;
	
	ControlRef prev = *slot;
	
	while ( prev )
	{
		ControlRef next = prev[0]->nextControl;
		
		if ( next == control )
		{
			slot = &prev[0]->nextControl;
			break;
		}
		
		prev = next;
	}
	
	if ( *slot == control )
	{
		*slot = control[0]->nextControl;
	}
	
	DisposeHandle( (Handle) control );
}

pascal void KillControls_patch( GrafPort* window )
{
	WindowPeek w = (WindowPeek) window;
	
	while ( ControlRef control = (ControlRef) w->controlList )
	{
		DisposeControl( control );
	}
	
	w->controlList = NULL;
}

#pragma mark -
#pragma mark Control Display
#pragma mark -

pascal void SetCTitle_patch( ControlRef control, ConstStr255Param title )
{
	fast_memcpy( control[0]->contrlTitle, title, 1 + title[ 0 ] );
	
	draw_control( control );
}

pascal void HideControl_patch( ControlRef control )
{
	if ( control[0]->contrlVis )
	{
		control[0]->contrlVis = 0;
		
		scoped_port thePort = control[0]->contrlOwner;
		
		// TODO:  Erase the region instead
		EraseRect( &control[0]->contrlRect );
		InvalRect( &control[0]->contrlRect );
	}
}

pascal void ShowControl_patch( ControlRef control )
{
	if ( ! control[0]->contrlVis )
	{
		control[0]->contrlVis = -1;
		
		draw_control( control );
	}
}

pascal void DrawControls_patch( GrafPort* window )
{
	WindowPeek w = (WindowPeek) window;
	
	ControlRef control = (ControlRef) w->controlList;
	
	while ( control != NULL )
	{
		draw_control( control );
		
		control = control[0]->nextControl;
	}
}

pascal void HiliteControl_patch( ControlRef control, short hiliteState )
{
	control[0]->contrlHilite = hiliteState;
	
	draw_control( control );
}

#pragma mark -
#pragma mark Mouse Location
#pragma mark -

pascal short FindControl_patch( Point pt, WindowRef window, ControlRef* which )
{
	*which = NULL;
	
	WindowPeek w = (WindowPeek) window;
	
	if ( ! w->visible )
	{
		return 0;
	}
	
	ControlRef next = (ControlRef) w->controlList;
	
	while ( next != NULL )
	{
		if ( short hit = TestControl( next, pt ) )
		{
			*which = next;
			
			return hit;
		}
		
		next = next[0]->nextControl;
	}
	
	return 0;
}

pascal short TrackControl_patch( ControlRecord**   control,
                                 Point             start,
                                 ControlActionUPP  action )
{
	static const RgnHandle tmp = (scoped_zone(), NewRgn());
	
	RgnHandle mouseRgn = tmp;
	
	WindowRef window = control[0]->contrlOwner;
	
	scoped_port thePort = window;
	
	const short csdx = window->portBits.bounds.left;
	const short csdy = window->portBits.bounds.top;
	
	if ( (long) action == -1 )
	{
		action = control[0]->contrlAction;
		
		if ( (long) action == -1 )
		{
			action = NULL;  // TODO:  Take action via CDEF
		}
	}
	
	Point pt = start;
	
	const short track_part = TestControl( control, pt );
	
	if ( track_part == 0 )
	{
		return 0;  // We hit a dead zone within the control bounds.
	}
	
	control[0]->contrlHilite = track_part;
	
	call_CDEF( control, drawCntl, track_part );
	
	long sleep = 0;
	
	if ( action )
	{
		mouseRgn = NULL;  // Don't leave WNE early for mouse-moved events
	}
	
	short hit = track_part;
	
	while ( true )
	{
		if ( action  &&  track_part < kControlIndicatorPart )
		{
			if ( hit == track_part )
			{
				action( control, track_part );
			}
			
			sleep = 6;
		}
		
		EventRecord event;
		
		const bool eventful = WaitNextEvent( mUpMask, &event, sleep, mouseRgn );
		
		// Events for DAs won't occur here, so we don't need to check.
		
		if ( event.what == mouseUp )
		{
			// global to local
			pt.h = event.where.h + csdx;
			pt.v = event.where.v + csdy;
			break;
		}
		
		Point where = event.where;
		
		if ( mouseRgn )
		{
			SetRectRgn( mouseRgn, where.h, where.v, where.h + 1, where.v + 1 );
		}
		
		if ( event.what != nullEvent )
		{
			/*
				Discard any mouse-moved events without redrawing.
				This can be prohibitively CPU-taxing on lightweight systems.
				
				Set sleep to 0 to ensure that we get a null event afterward.
			*/
			
			sleep = 0;
			
			continue;
		}
		
		/*
			We got a null event -- redraw the control (if it changed status,
			that is), and set sleep back to forever (approx).
		*/
		
		sleep = 0xFFFFFFFF;
		
		// global to local
		where.h += csdx;
		where.v += csdy;
		
		if ( action  ||  *(long*) &pt != *(long*) &where )
		{
			pt = where;
			
			hit = TestControl( control, pt );
			
			if ( hit != track_part )
			{
				hit = 0;
			}
			
			if ( hit != control[0]->contrlHilite )
			{
				control[0]->contrlHilite = hit;
				
				call_CDEF( control, drawCntl, track_part );
			}
		}
	}
	
	control[0]->contrlHilite = 0;
	
	hit = TestControl( control, pt );
	
	if ( hit == track_part )
	{
		call_CDEF( control, drawCntl, hit );
		
		return hit;
	}
	
	return 0;
}

pascal short TestControl_patch( ControlRecord** control, Point pt )
{
	if ( control[0]->contrlVis  &&  control[0]->contrlHilite != 255 )
	{
		scoped_port thePort = control[0]->contrlOwner;
		
		long hit = call_CDEF( control, testCntl, *(long*) &pt );
		
		return hit;
	}
	
	return 0;
}

#pragma mark -
#pragma mark Control Movement and Sizing
#pragma mark -

pascal void MoveControl_patch( ControlRecord** control, short h, short v )
{
	Rect* bounds = &control[0]->contrlRect;
	
	const bool visible = control[0]->contrlVis;
	
	if ( visible )
	{
		HideControl_patch( control );
	}
	
	OffsetRect( bounds, h - bounds->left, v - bounds->top );
	
	if ( visible )
	{
		ShowControl_patch( control );
	}
}

pascal void SizeControl_patch( ControlRecord** control, short w, short h )
{
	Rect* bounds = &control[0]->contrlRect;
	
	const bool visible = control[0]->contrlVis;
	
	if ( visible )
	{
		HideControl_patch( control );
	}
	
	bounds->right = bounds->left + w;
	bounds->bottom = bounds->top + h;
	
	if ( visible )
	{
		ShowControl_patch( control );
	}
}

#pragma mark -
#pragma mark Control Setting and Range
#pragma mark -

pascal void SetCtlValue_patch( ControlRecord** control, short value )
{
	const short min = control[0]->contrlMin;
	const short max = control[0]->contrlMax;
	
	if ( min < max )
	{
		if ( value > max )
		{
			value = max;
		}
		else if ( value < min )
		{
			value = min;
		}
		
		control[0]->contrlValue = value;
		
		draw_control( control, kDrawControlIndicatorOnly );
	}
}

pascal short GetCtlValue_patch( ControlRecord** control )
{
	return control[0]->contrlValue;
}

pascal void SetMinCtl_patch( ControlRecord** control, short min )
{
	if ( min != control[0]->contrlMin  &&  min <= control[0]->contrlMax )
	{
		if ( control[0]->contrlValue < min )
		{
			control[0]->contrlValue = min;
		}
		
		control[0]->contrlMin = min;
		
		draw_control( control, kDrawControlIndicatorOnly );
	}
}

pascal short GetMinCtl_patch( ControlRecord** control )
{
	return control[0]->contrlMin;
}

pascal void SetMaxCtl_patch( ControlRecord** control, short max )
{
	if ( max != control[0]->contrlMax  &&  max >= control[0]->contrlMin )
	{
		if ( control[0]->contrlValue > max )
		{
			control[0]->contrlValue = max;
		}
		
		control[0]->contrlMax = max;
		
		draw_control( control, kDrawControlIndicatorOnly );
	}
}

pascal short GetMaxCtl_patch( ControlRecord** control )
{
	return control[0]->contrlMax;
}

#pragma mark -
#pragma mark Miscellaneous Routines
#pragma mark -

pascal void SetCRefCon_patch( ControlRecord** control, long refCon )
{
	control[0]->contrlRfCon = refCon;
}

pascal long GetCRefCon_patch( ControlRecord** control )
{
	return control[0]->contrlRfCon;
}

pascal void SetCtlAction_patch( ControlRecord** control, ControlActionProcPtr action )
{
	control[0]->contrlAction = action;
}

pascal ControlActionProcPtr GetCtlAction_patch( ControlRecord** control )
{
	return control[0]->contrlAction;
}

#pragma mark -
#pragma mark 128K ROM Additions
#pragma mark -

pascal void UpdateControls_patch( GrafPort* window, MacRegion** updateRgn )
{
	DrawControls_patch( window );
}

pascal void Draw1Control_patch( ControlRecord** control )
{
	draw_control( control );
}

pascal short GetCVariant_patch( ControlRecord** control )
{
	const short varCode = *(Byte*) &control[0]->contrlDefProc;
	
	return varCode;
}
