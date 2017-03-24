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

// Standard C
#include <string.h>

// ams-core
#include "CDEF.hh"


pascal ControlRecord** NewControl_patch( GrafPort*             window,
                                         const Rect*           bounds,
                                         const unsigned char*  title,
                                         short                 visible,
                                         short                 value,
                                         short                 min,
                                         short                 max,
                                         short                 procID,
                                         long                  refCon )
{
	ControlRef control = (ControlRef) NewHandleClear( sizeof (ControlRecord) );
	
	if ( control )
	{
		WindowPeek w = (WindowPeek) window;
		
		control[0]->nextControl   = (ControlRef) w->controlList;
		control[0]->contrlOwner   = window;
		control[0]->contrlRect    = *bounds;
		control[0]->contrlVis     = -!! visible;
	//	control[0]->contrlHilite  = 0;
		control[0]->contrlValue   = value;
		control[0]->contrlMin     = min;
		control[0]->contrlMax     = max;
		control[0]->contrlDefProc = NULL;  // TODO
	//	control[0]->contrlData    = NULL;
	//	control[0]->contrlAction  = NULL;
		control[0]->contrlRfCon   = refCon;
		
		if ( title )
		{
			memcpy( control[0]->contrlTitle, title, 1 + title[ 0 ] );
		}
		
		const short varCode = procID & 0x0F;
		
		*(Byte*) &control[0]->contrlDefProc = varCode;
		
		w->controlList = (Handle) control;
		
		if ( visible )
		{
			CDEF_0( varCode, control, drawCntl, 0 );
		}
	}
	
	return control;
}

pascal void DisposeControl_patch( ControlRecord** control )
{
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
}

pascal short TrackControl_patch( ControlRecord**  control,
                                 Point            start,
                                 pascal void    (*action)() )
{
	RgnHandle tmp = NewRgn();
	RgnHandle mouseRgn = tmp;
	
	WindowRef window = control[0]->contrlOwner;
	
	const short csdx = window->portBits.bounds.left;
	const short csdy = window->portBits.bounds.top;
	
	const short varCode = *(Byte*) &control[0]->contrlDefProc;
	
	Point pt = start;
	
	const short track_part = CDEF_0( varCode, control, testCntl, *(long*) &pt );
	
	control[0]->contrlHilite = track_part;
	
	CDEF_0( varCode, control, drawCntl, track_part );
	
	long sleep = 0;
	
	while ( true )
	{
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
		
		SetRectRgn( mouseRgn, where.h, where.v, where.h + 1, where.v + 1 );
		
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
		
		sleep = 0x7fffffff;
		
		// global to local
		where.h += csdx;
		where.v += csdy;
		
		if ( *(long*) &pt != *(long*) &where )
		{
			pt = where;
			
			short hit = CDEF_0( varCode, control, testCntl, *(long*) &pt );
			
			if ( hit != track_part )
			{
				hit = 0;
			}
			
			if ( hit != control[0]->contrlHilite )
			{
				control[0]->contrlHilite = hit;
				
				CDEF_0( varCode, control, drawCntl, track_part );
			}
		}
	}
	
	DisposeRgn( tmp );
	
	control[0]->contrlHilite = 0;
	
	const short hit = CDEF_0( varCode, control, testCntl, *(long*) &pt );
	
	if ( hit == track_part )
	{
		CDEF_0( varCode, control, drawCntl, hit );
		
		return hit;
	}
	
	return 0;
}

pascal void DrawControls_patch( GrafPort* window )
{
	WindowPeek w = (WindowPeek) window;
	
	ControlRef control = (ControlRef) w->controlList;
	
	while ( control != NULL )
	{
		const short varCode = *(Byte*) &control[0]->contrlDefProc;
		
		CDEF_0( varCode, control, drawCntl, 0 );
		
		control = control[0]->nextControl;
	}
}

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
		const short varCode = *(Byte*) &next[0]->contrlDefProc;
		
		if ( long hit = CDEF_0( varCode, next, testCntl, *(long*) &pt ) )
		{
			*which = next;
			
			return hit;
		}
		
		next = next[0]->nextControl;
	}
	
	return 0;
}
