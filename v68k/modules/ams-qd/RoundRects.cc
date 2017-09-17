/*
	RoundRects.cc
	-------------
*/

#include "RoundRects.hh"

// Mac OS
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif

// ams-common
#include "QDGlobals.hh"

// ams-qd
#include "RoundRectRgn.hh"


pascal void StdRRect_patch( signed char  verb,
                            const Rect*  r,
                            short        ovalWidth,
                            short        ovalHeight )
{
	const short width  = r->right - r->left;
	const short height = r->bottom - r->top;
	
	if ( width <= 0  ||  height <= 0 )
	{
		return;
	}
	
	RgnHandle rgn = NewRgn();
	
	if ( verb == kQDGrafVerbFrame )
	{
		verb = kQDGrafVerbPaint;
		
		GrafPort& port = **get_addrof_thePort();
		
		if ( RgnHandle rgnSave = (RgnHandle) port.rgnSave )
		{
			RoundRectRgn( rgn, width, height, ovalWidth, ovalHeight );
			
			UnionRgn( rgn, rgnSave, rgnSave );
			
			EmptyRgn( rgn );
		}
		
		const short penHeight = port.pnSize.v;
		const short penWidth  = port.pnSize.h;
		
		FramedRoundRectRgn( rgn,
		                    width,
		                    height,
		                    ovalWidth,
		                    ovalHeight,
		                    penWidth,
		                    penHeight );
	}
	else
	{
		RoundRectRgn( rgn, width, height, ovalWidth, ovalHeight );
	}
	
	OffsetRgn( rgn, r->left, r->top );
	
	StdRgn( verb, rgn );
	
	DisposeRgn( rgn );
}

pascal void EraseRoundRect_patch( const Rect*  rect,
                                  short        ovalWidth,
                                  short        ovalHeight )
{
	StdRRect( kQDGrafVerbErase, rect, ovalWidth, ovalHeight );
}

pascal void PaintRoundRect_patch( const Rect*  rect,
                                  short        ovalWidth,
                                  short        ovalHeight )
{
	StdRRect( kQDGrafVerbPaint, rect, ovalWidth, ovalHeight );
}

pascal void InverRoundRect_patch( const Rect*  rect,
                                  short        ovalWidth,
                                  short        ovalHeight )
{
	StdRRect( kQDGrafVerbInvert, rect, ovalWidth, ovalHeight );
}

pascal void FrameRoundRect_patch( const Rect*  rect,
                                  short        ovalWidth,
                                  short        ovalHeight )
{
	StdRRect( kQDGrafVerbFrame, rect, ovalWidth, ovalHeight );
}

pascal void FillRoundRect_patch( const Rect*     rect,
                                 short           ovalWidth,
                                 short           ovalHeight,
                                 const Pattern*  pattern )
{
	GrafPort& port = **get_addrof_thePort();
	
	port.fillPat = *pattern;
	
	StdRRect( kQDGrafVerbFill, rect, ovalWidth, ovalHeight );
}
