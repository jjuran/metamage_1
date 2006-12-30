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

CARBONUNITS_LINKAGE PixMapHandle GetPortPixMap( CGrafPtr port )
{
	return port->portPixMap;
}

CARBONUNITS_LINKAGE BitMap* GetPortBitMapForCopyBits( CGrafPtr cport )
{
	GrafPtr port = reinterpret_cast< GrafPtr >( cport );
	
	return &port->portBits;
}

CARBONUNITS_LINKAGE Rect* GetPortBounds( CGrafPtr port, Rect* bounds )
{
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

CARBONUNITS_LINKAGE RGBColor* GetPortOpColor( CGrafPtr port, RGBColor* color )
{
	Handle h = port->grafVars;
	
	GVarHandle grafVars = reinterpret_cast< GVarHandle >( h );
	
	*color = grafVars[0]->rgbOpColor;
	
	return color;
}

CARBONUNITS_LINKAGE RGBColor* GetPortHiliteColor( CGrafPtr port, RGBColor* color )
{
	Handle h = port->grafVars;
	
	GVarHandle grafVars = reinterpret_cast< GVarHandle >( h );
	
	*color = grafVars[0]->rgbHiliteColor;
	
	return color;
}

CARBONUNITS_LINKAGE CQDProcsPtr GetPortGrafProcs( CGrafPtr port )
{
	return port->grafProcs;
}

CARBONUNITS_LINKAGE short GetPortTextFont( CGrafPtr port )
{
	return port->txFont;
}

CARBONUNITS_LINKAGE short GetPortTextFace( CGrafPtr port )
{
	return port->txFace;
}

CARBONUNITS_LINKAGE short GetPortTextMode( CGrafPtr port )
{
	return port->txMode;
}

CARBONUNITS_LINKAGE short GetPortTextSize( CGrafPtr port )
{
	return port->txSize;
}

CARBONUNITS_LINKAGE short GetPortChExtra( CGrafPtr port )
{
	return port->chExtra;
}

CARBONUNITS_LINKAGE short GetPortFracHPenLocation( CGrafPtr port )
{
	return port->pnLocHFrac;
}

CARBONUNITS_LINKAGE Fixed GetPortSpExtra( CGrafPtr port )
{
	return port->spExtra;
}

CARBONUNITS_LINKAGE short GetPortPenVisibility( CGrafPtr port )
{
	return port->pnVis;
}

CARBONUNITS_LINKAGE RgnHandle GetPortVisibleRegion( CGrafPtr port, RgnHandle region )
{
	MacCopyRgn( port->visRgn, region );
	
	return region;
}

CARBONUNITS_LINKAGE RgnHandle GetPortClipRegion( CGrafPtr port, RgnHandle region )
{
	MacCopyRgn( port->clipRgn, region );
	
	return region;
}

CARBONUNITS_LINKAGE PixPatHandle GetPortBackPixPat( CGrafPtr port, PixPatHandle backPattern )
{
	CopyPixPat( port->bkPixPat, backPattern );
	
	return backPattern;
}

CARBONUNITS_LINKAGE PixPatHandle GetPortPenPixPat( CGrafPtr port, PixPatHandle penPattern )
{
	CopyPixPat( port->pnPixPat, penPattern );
	
	return penPattern;
}

CARBONUNITS_LINKAGE PixPatHandle GetPortFillPixPat( CGrafPtr port, PixPatHandle fillPattern )
{
	CopyPixPat( port->fillPixPat, fillPattern );
	
	return fillPattern;
}

CARBONUNITS_LINKAGE Point* GetPortPenSize( CGrafPtr port, Point* penSize )
{
	*penSize = port->pnSize;
	
	return penSize;
}

CARBONUNITS_LINKAGE SInt32 GetPortPenMode( CGrafPtr port )
{
	return port->pnMode;
}

CARBONUNITS_LINKAGE Point* GetPortPenLocation( CGrafPtr port, Point* penLocation )
{
	*penLocation = port->pnLoc;
	
	return penLocation;
}

CARBONUNITS_LINKAGE Boolean IsPortRegionBeingDefined( CGrafPtr port )
{
	return port->rgnSave != NULL;
}

CARBONUNITS_LINKAGE Boolean IsPortPictureBeingDefined( CGrafPtr port )
{
	return port->picSave != NULL;
}

CARBONUNITS_LINKAGE Boolean IsPortPolyBeingDefined( CGrafPtr port )
{
	return port->polySave != NULL;
}

// IsPortOffscreen

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

// SetPortBounds
// SetPortOpColor
// SetPortGrafProcs

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

CARBONUNITS_LINKAGE void SetPortVisibleRegion( CGrafPtr port, RgnHandle visRgn )
{
	MacCopyRgn( visRgn, port->visRgn );
}

CARBONUNITS_LINKAGE void SetPortClipRegion( CGrafPtr port, RgnHandle clipRgn )
{
	MacCopyRgn( clipRgn, port->clipRgn );
}

// SetPortPenPixPat
// SetPortFillPixPat
// SetPortBackPixPat

CARBONUNITS_LINKAGE void SetPortPenSize( CGrafPtr port, Point penSize )
{
	port->pnSize = penSize;
}

CARBONUNITS_LINKAGE void SetPortPenMode( CGrafPtr port, SInt32 penMode )
{
	port->pnMode = penMode;
}

CARBONUNITS_LINKAGE void SetPortFracHPenLocation( CGrafPtr port, short pnLocHFrac )
{
	port->pnLocHFrac = pnLocHFrac;
}

CARBONUNITS_LINKAGE Rect* GetPixBounds( PixMapHandle pixMap, Rect* bounds )
{
	*bounds = pixMap[0]->bounds;
	
	return bounds;
}

// GetPixDepth

CARBONUNITS_LINKAGE long GetQDGlobalsRandomSeed()
{
	return qd.randSeed;
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

CARBONUNITS_LINKAGE void SetQDGlobalsRandomSeed( long randomSeed )
{
	qd.randSeed = randomSeed;
}

CARBONUNITS_LINKAGE void SetQDGlobalsArrow( const Cursor* arrow )
{
	qd.arrow = *arrow;
}

CARBONUNITS_LINKAGE Rect* GetRegionBounds( RgnHandle region, Rect* bounds )
{
	*bounds = region[0]->rgnBBox;
	
	return bounds;
}

// IsRegionRectangular

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

// SetQDError

#undef CARBONUNITS_LINKAGE

#endif

#endif

