/*
	Regions.cc
	----------
*/

#include "Regions.hh"

// Mac OS
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif

// ams-common
#include "callouts.hh"
#include "QDGlobals.hh"
#include "scoped_zone.hh"

// ams-qd
#include "GrafProcs.hh"
#include "Rects.hh"


#pragma exceptions off


pascal void StdRgn_patch( signed char verb, RgnHandle rgn )
{
	static RgnHandle utility_rgn = (scoped_zone(), NewRgn());
	
	GrafPort& port = *get_thePort();
	
	if ( verb == kQDGrafVerbFrame )
	{
		RgnHandle frame = utility_rgn;  // clobbered by CopyRgn()
		
		CopyRgn( rgn, frame );
		
		if ( RgnHandle rgnSave = (RgnHandle) port.rgnSave )
		{
			UnionRgn( rgn, rgnSave, rgnSave );
		}
		
		InsetRgn( frame, port.pnSize.h, port.pnSize.v );
		
		DiffRgn( rgn, frame, frame );
		
		verb = kQDGrafVerbPaint;
		
		rgn = frame;
	}
	
	RgnHandle clipRgn = utility_rgn;  // clobbered by SectRgn()
	
	SectRgn( port.clipRgn, rgn, clipRgn );
	
	RgnHandle saved_clipRgn = port.clipRgn;
	
	port.clipRgn = clipRgn;
	
	const Rect& box = clipRgn[0]->rgnBBox;
	
	StdRect_patch( verb, &box );
	
	port.clipRgn = saved_clipRgn;
}

pascal void FrameRgn_patch( RgnHandle rgn )
{
	GrafPort& port = *get_thePort();
	
	CHECK_CUSTOM_GRAFPROC( port, rgnProc );
	
	GRAFPROC_RGN( port )( kQDGrafVerbFrame, rgn );
}

pascal void PaintRgn_patch( RgnHandle rgn )
{
	GrafPort& port = *get_thePort();
	
	CHECK_CUSTOM_GRAFPROC( port, rgnProc );
	
	GRAFPROC_RGN( port )( kQDGrafVerbPaint, rgn );
}

pascal void EraseRgn_patch( RgnHandle rgn )
{
	GrafPort& port = *get_thePort();
	
	CHECK_CUSTOM_GRAFPROC( port, rgnProc );
	
	GRAFPROC_RGN( port )( kQDGrafVerbErase, rgn );
}

pascal void InverRgn_patch( RgnHandle rgn )
{
	GrafPort& port = *get_thePort();
	
	CHECK_CUSTOM_GRAFPROC( port, rgnProc );
	
	GRAFPROC_RGN( port )( kQDGrafVerbInvert, rgn );
}

pascal void FillRgn_patch( RgnHandle rgn, const Pattern* pattern )
{
	GrafPort& port = *get_thePort();
	
	port.fillPat = *pattern;
	
	CHECK_CUSTOM_GRAFPROC( port, rgnProc );
	
	GRAFPROC_RGN( port )( kQDGrafVerbFill, rgn );
}
