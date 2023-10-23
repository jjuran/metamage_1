/*
	CopyBits.cc
	-----------
*/

#include "CopyBits.hh"

// Mac OS
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif

// ams-common
#include "callouts.hh"
#include "QDGlobals.hh"
#include "scoped_zone.hh"

// ams-qd
#include "utility_region.hh"


#pragma exceptions off


static inline
bool equal_bitmaps( const BitMap* a, const BitMap* b )
{
	return a == b  ||  fast_memcmp( a, b, sizeof (BitMap) ) == 0;
}

static
GrafPtr new_port()
{
	scoped_zone null_zone;
	
	GrafPtr port = (GrafPtr) NewPtr( sizeof (GrafPort) );
	
	OpenPort( port );
	
	return port;
}

pascal void CopyBits_patch( const BitMap*  srcBits,
                            const BitMap*  dstBits,
                            const Rect*    srcRect,
                            const Rect*    dstRect,
                            short          mode,
                            MacRegion**    maskRgn )
{
	GrafPtr& thePort = *get_addrof_thePort();
	
	const GrafPtr saved_port = thePort;
	
	if ( ! equal_bitmaps( dstBits, &thePort->portBits ) )
	{
		static GrafPtr port = new_port();
		
		thePort = port;
		
		SetPortBits( dstBits );
	}
	
	StdBits( srcBits, srcRect, dstRect, mode, maskRgn );
	
	thePort = saved_port;
}

pascal void ScrollRect_patch( const Rect*  rect,
                              short        dh,
                              short        dv,
                              MacRegion**  updateRgn )
{
	GrafPtr thePort = *get_addrof_thePort();
	
	const Rect& srcRect = *rect;
	
	Rect dstRect = srcRect;
	
	// Start with the visRgn (omitting any occluded parts of the source).
	CopyRgn( thePort->visRgn, updateRgn );
	
	// Translate the visRgn (along with the dstRect itself).
	OffsetRgn( updateRgn, dh, dv );
	OffsetRect( &dstRect, dh, dv );
	
	SectRgn( updateRgn, rectangular_utility_region( *rect ), updateRgn );
	
	// Use the translated visRgn as a mask region to omit occluded bits.
	
	CopyBits( &thePort->portBits,
	          &thePort->portBits,
	          &srcRect,
	          &dstRect,
	          srcCopy,
	          updateRgn );
	
	// The actual update region is (srcRect - dstRect).
	
	RectRgn( updateRgn, &srcRect );
	
	DiffRgn( updateRgn, rectangular_utility_region( dstRect ), updateRgn );
	
	EraseRgn( updateRgn );
}
