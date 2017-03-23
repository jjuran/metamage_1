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
