/*
	Ovals.cc
	--------
*/

#include "Ovals.hh"

// Mac OS
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif

// ams-common
#include "QDGlobals.hh"
#include "scoped_zone.hh"

// ams-qd
#include "GrafProcs.hh"
#include "OvalRgn.hh"
#include "Regions.hh"


#pragma exceptions off


pascal void StdOval_patch( signed char verb, const Rect* r )
{
	const short width  = r->right - r->left;
	const short height = r->bottom - r->top;
	
	if ( width <= 0  ||  height <= 0 )
	{
		return;
	}
	
	if ( width < 3  ||  height < 3 )
	{
		StdRect( verb, r );
		return;
	}
	
	static RgnHandle ovalRgn = (scoped_zone(), NewRgn());
	
	if ( verb == kQDGrafVerbFrame )
	{
		verb = kQDGrafVerbPaint;
		
		const GrafPort& port = *get_thePort();
		
		if ( RgnHandle rgnSave = (RgnHandle) port.rgnSave )
		{
			OvalRgn( ovalRgn, width, height );
			
			OffsetRgn( ovalRgn, r->left, r->top );
			
			UnionRgn( ovalRgn, rgnSave, rgnSave );
			
			SetEmptyRgn( ovalRgn );
		}
		
		const short penHeight = port.pnSize.v;
		const short penWidth  = port.pnSize.h;
		
		FramedOvalRgn( ovalRgn, width, height, penWidth, penHeight );
	}
	else
	{
		OvalRgn( ovalRgn, width, height );
	}
	
	OffsetRgn( ovalRgn, r->left, r->top );
	
	StdRgn_patch( verb, ovalRgn );
	
	SetEmptyRgn( ovalRgn );
}

pascal void EraseOval_patch( const Rect* rect )
{
	GrafPort& port = *get_thePort();
	
	CHECK_CUSTOM_GRAFPROC( port, ovalProc );
	
	GRAFPROC_OVAL( port )( kQDGrafVerbErase, rect );
}

pascal void PaintOval_patch( const Rect* rect )
{
	GrafPort& port = *get_thePort();
	
	CHECK_CUSTOM_GRAFPROC( port, ovalProc );
	
	GRAFPROC_OVAL( port )( kQDGrafVerbPaint, rect );
}

pascal void InvertOval_patch( const Rect* rect )
{
	GrafPort& port = *get_thePort();
	
	CHECK_CUSTOM_GRAFPROC( port, ovalProc );
	
	GRAFPROC_OVAL( port )( kQDGrafVerbInvert, rect );
}

pascal void FrameOval_patch( const Rect* rect )
{
	GrafPort& port = *get_thePort();
	
	CHECK_CUSTOM_GRAFPROC( port, ovalProc );
	
	GRAFPROC_OVAL( port )( kQDGrafVerbFrame, rect );
}

pascal void FillOval_patch( const Rect* rect, const Pattern* pattern )
{
	GrafPort& port = *get_thePort();
	
	port.fillPat = *pattern;
	
	CHECK_CUSTOM_GRAFPROC( port, ovalProc );
	
	GRAFPROC_OVAL( port )( kQDGrafVerbFill, rect );
}
