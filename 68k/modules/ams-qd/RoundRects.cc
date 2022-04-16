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
#include "scoped_zone.hh"

// ams-qd
#include "RoundRectRgn.hh"


#pragma exceptions off


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
	
	static RgnHandle rgn = (scoped_zone(), NewRgn());
	
	if ( verb == kQDGrafVerbFrame )
	{
		verb = kQDGrafVerbPaint;
		
		GrafPort& port = **get_addrof_thePort();
		
		if ( RgnHandle rgnSave = (RgnHandle) port.rgnSave )
		{
			RoundRectRgn( rgn, width, height, ovalWidth, ovalHeight );
			
			OffsetRgn( rgn, r->left, r->top );
			
			UnionRgn( rgn, rgnSave, rgnSave );
			
			SetEmptyRgn( rgn );
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
	
	SetEmptyRgn( rgn );
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
