// CarbonUnits/Quickdraw.hh

#ifndef CARBONUNITS_QUICKDRAW_HH
#define CARBONUNITS_QUICKDRAW_HH

#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif

#if TARGET_API_MAC_CARBON
#error Configuration error:  This file is for classic only
#endif

// We need these definitions under the following conditions:
// 1. Accessors are functions on 68K (no CarbonAccessors.o), for compiling
// 2. Accessors are not functions, for including

// CARBONUNITS_LINKAGE is only defined (to 'pascal') if we're compiling.

#if defined(CARBONUNITS_LINKAGE) && ACCESSOR_CALLS_ARE_FUNCTIONS && TARGET_CPU_68K || !defined(CARBONUNITS_LINKAGE) && !ACCESSOR_CALLS_ARE_FUNCTIONS

#ifndef CARBONUNITS_LINKAGE
#define CARBONUNITS_LINKAGE inline pascal
#endif

CARBONUNITS_LINKAGE BitMap* GetPortBitMapForCopyBits( CGrafPtr cport )
{
	GrafPtr port = reinterpret_cast< GrafPtr >( cport );
	
	return &port->portBits;
}

CARBONUNITS_LINKAGE Rect* GetPortBounds( CGrafPtr cport, Rect* bounds )
{
	GrafPtr port = reinterpret_cast< GrafPtr >( cport );
	
	*bounds = port->portRect;
	
	return bounds;
}

CARBONUNITS_LINKAGE RGBColor* GetPortForeColor( CGrafPtr port, RGBColor* color )
{
	*color = port->rgbFgColor;
	
	return color;
}

CARBONUNITS_LINKAGE RGBColor* GetPortBackColor( CGrafPtr port, RGBColor* color )
{
	*color = port->rgbBkColor;
	
	return color;
}

CARBONUNITS_LINKAGE RgnHandle GetPortVisibleRegion( CGrafPtr cport, RgnHandle region )
{
	GrafPtr port = reinterpret_cast< GrafPtr >( cport );
	
	MacCopyRgn( port->visRgn, region );
	
	return region;
}

CARBONUNITS_LINKAGE RgnHandle GetPortClipRegion( CGrafPtr cport, RgnHandle region )
{
	GrafPtr port = reinterpret_cast< GrafPtr >( cport );
	
	MacCopyRgn( port->clipRgn, region );
	
	return region;
}

CARBONUNITS_LINKAGE Point* GetPortPenSize( CGrafPtr cport, Point* penSize )
{
	GrafPtr port = reinterpret_cast< GrafPtr >( cport );
	
	*penSize = port->pnSize;
	
	return penSize;
}

CARBONUNITS_LINKAGE Boolean IsPortColor( CGrafPtr port )
{
	// Taken from QISA/MoreIsBetter/MoreQuickDraw.cp
	return port->portVersion < 0;
}

CARBONUNITS_LINKAGE Boolean IsPortVisibleRegionEmpty( CGrafPtr port )
{
	return EmptyRgn( port->visRgn );
}

CARBONUNITS_LINKAGE Boolean IsPortClipRegionEmpty( CGrafPtr port )
{
	return EmptyRgn( port->clipRgn );
}

// SectRegionWithPortClipRegion
// SectRegionWithPortVisibleRegion

CARBONUNITS_LINKAGE Handle SwapPortPicSaveHandle( CGrafPtr port, Handle newHandle )
{
	Handle oldHandle = port->picSave;
	
	port->picSave = newHandle;
	
	return oldHandle;
}

CARBONUNITS_LINKAGE Handle SwapPortPolySaveHandle( CGrafPtr port, Handle newHandle )
{
	Handle oldHandle = port->polySave;
	
	port->polySave = newHandle;
	
	return oldHandle;
}

CARBONUNITS_LINKAGE Handle SwapPortRegionSaveHandle( CGrafPtr port, Handle newHandle )
{
	Handle oldHandle = port->rgnSave;
	
	port->rgnSave = newHandle;
	
	return oldHandle;
}

CARBONUNITS_LINKAGE void SetPortTextFont( CGrafPtr port, short txFont )
{
	port->txFont = txFont;
}

CARBONUNITS_LINKAGE void SetPortTextSize( CGrafPtr port, short txSize )
{
	port->txSize = txSize;
}

CARBONUNITS_LINKAGE void SetPortTextFace( CGrafPtr port, StyleParameter face )
{
	port->txFace = face;
}

CARBONUNITS_LINKAGE void SetPortTextMode( CGrafPtr port, short mode )
{
	port->txMode = mode;
}

CARBONUNITS_LINKAGE void SetPortClipRegion( CGrafPtr cport, RgnHandle clipRgn )
{
	GrafPtr port = reinterpret_cast< GrafPtr >( cport );
	
	MacCopyRgn( clipRgn, port->clipRgn );
}

CARBONUNITS_LINKAGE void SetPortPenSize( CGrafPtr cport, Point penSize )
{
	GrafPtr port = reinterpret_cast< GrafPtr >( cport );
	
	port->pnSize = penSize;
}

CARBONUNITS_LINKAGE BitMap* GetQDGlobalsScreenBits( BitMap* result )
{
	*result = qd.screenBits;
	
	return result;
}

CARBONUNITS_LINKAGE Cursor* GetQDGlobalsArrow( Cursor* result )
{
	*result = qd.arrow;
	
	return result;
}

CARBONUNITS_LINKAGE Pattern* GetQDGlobalsDarkGray( Pattern* result )
{
	*result = qd.dkGray;
	
	return result;
}

CARBONUNITS_LINKAGE Pattern* GetQDGlobalsLightGray( Pattern* result )
{
	*result = qd.ltGray;
	
	return result;
}

CARBONUNITS_LINKAGE Pattern* GetQDGlobalsGray( Pattern* result )
{
	*result = qd.gray;
	
	return result;
}

CARBONUNITS_LINKAGE Pattern* GetQDGlobalsBlack( Pattern* result )
{
	*result = qd.black;
	
	return result;
}

CARBONUNITS_LINKAGE Pattern* GetQDGlobalsWhite( Pattern* result )
{
	*result = qd.white;
	
	return result;
}

CARBONUNITS_LINKAGE CGrafPtr GetQDGlobalsThePort()
{
	return reinterpret_cast< CGrafPtr >( qd.thePort );
}

CARBONUNITS_LINKAGE CGrafPtr CreateNewPort()
{
	Ptr portMem = NewPtr( sizeof (CGrafPort) );
	
	if ( portMem == NULL )  return NULL;
	
	CGrafPtr port = reinterpret_cast< CGrafPtr >( portMem );
	
	OpenCPort( port );
	
	return port;
}

CARBONUNITS_LINKAGE void DisposePort( CGrafPtr port )
{
	CloseCPort( port );
	
	DisposePtr( reinterpret_cast< Ptr >( port ) );
}

#undef CARBONUNITS_LINKAGE

#endif

#endif

