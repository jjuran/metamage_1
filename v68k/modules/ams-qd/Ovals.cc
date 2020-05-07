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
#include "OvalRgn.hh"


pascal void StdOval_patch( signed char verb, const Rect* r )
{
	const short width  = r->right - r->left;
	const short height = r->bottom - r->top;
	
	if ( width <= 0  ||  height <= 0 )
	{
		return;
	}
	
	static RgnHandle ovalRgn = (scoped_zone(), NewRgn());
	
	if ( verb == kQDGrafVerbFrame )
	{
		verb = kQDGrafVerbPaint;
		
		GrafPort& port = **get_addrof_thePort();
		
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
	
	StdRgn( verb, ovalRgn );
	
	SetEmptyRgn( ovalRgn );
}

pascal void EraseOval_patch( const Rect* rect )
{
	StdOval( kQDGrafVerbErase, rect );
}

pascal void PaintOval_patch( const Rect* rect )
{
	StdOval( kQDGrafVerbPaint, rect );
}

pascal void InvertOval_patch( const Rect* rect )
{
	StdOval( kQDGrafVerbInvert, rect );
}

pascal void FrameOval_patch( const Rect* rect )
{
	StdOval( kQDGrafVerbFrame, rect );
}

pascal void FillOval_patch( const Rect* rect, const Pattern* pattern )
{
	GrafPort& port = **get_addrof_thePort();
	
	port.fillPat = *pattern;
	
	StdOval( kQDGrafVerbFill, rect );
}
