// Carbonate/Quickdraw.hh

#ifndef CARBONATE_QUICKDRAW_HH
#define CARBONATE_QUICKDRAW_HH

#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif

#if TARGET_API_MAC_CARBON
#error Configuration error:  This file is for classic only
#endif

// We need these definitions under the following conditions:
// 1. Accessors are functions on 68K (no CarbonAccessors.o), for compiling
// 2. Accessors are not functions, for including

// CARBONATE_LINKAGE is only defined (to 'pascal') if we're compiling.

#if defined(CARBONATE_LINKAGE) && ACCESSOR_CALLS_ARE_FUNCTIONS && TARGET_CPU_68K || !defined(CARBONATE_LINKAGE) && !ACCESSOR_CALLS_ARE_FUNCTIONS

#ifndef CARBONATE_LINKAGE
#define CARBONATE_LINKAGE inline pascal
#endif

CARBONATE_LINKAGE PixMapHandle GetPortPixMap( CGrafPtr port )
{
	return port->portPixMap;
}

CARBONATE_LINKAGE BitMap* GetPortBitMapForCopyBits( CGrafPtr cport )
{
	GrafPtr port = reinterpret_cast< GrafPtr >( cport );
	
	return &port->portBits;
}

CARBONATE_LINKAGE Rect* GetPortBounds( CGrafPtr port, Rect* bounds )
{
	*bounds = port->portRect;
	
	return bounds;
}

CARBONATE_LINKAGE RGBColor* GetPortForeColor( CGrafPtr port, RGBColor* color )
{
	*color = port->rgbFgColor;
	
	return color;
}

CARBONATE_LINKAGE RGBColor* GetPortBackColor( CGrafPtr port, RGBColor* color )
{
	*color = port->rgbBkColor;
	
	return color;
}

CARBONATE_LINKAGE RGBColor* GetPortOpColor( CGrafPtr port, RGBColor* color )
{
	Handle h = port->grafVars;
	
	GVarHandle grafVars = reinterpret_cast< GVarHandle >( h );
	
	*color = grafVars[0]->rgbOpColor;
	
	return color;
}

CARBONATE_LINKAGE RGBColor* GetPortHiliteColor( CGrafPtr port, RGBColor* color )
{
	Handle h = port->grafVars;
	
	GVarHandle grafVars = reinterpret_cast< GVarHandle >( h );
	
	*color = grafVars[0]->rgbHiliteColor;
	
	return color;
}

CARBONATE_LINKAGE CQDProcsPtr GetPortGrafProcs( CGrafPtr port )
{
	return port->grafProcs;
}

CARBONATE_LINKAGE short GetPortTextFont( CGrafPtr port )
{
	return port->txFont;
}

CARBONATE_LINKAGE short GetPortTextFace( CGrafPtr port )
{
	return port->txFace;
}

CARBONATE_LINKAGE short GetPortTextMode( CGrafPtr port )
{
	return port->txMode;
}

CARBONATE_LINKAGE short GetPortTextSize( CGrafPtr port )
{
	return port->txSize;
}

CARBONATE_LINKAGE short GetPortChExtra( CGrafPtr port )
{
	return port->chExtra;
}

CARBONATE_LINKAGE short GetPortFracHPenLocation( CGrafPtr port )
{
	return port->pnLocHFrac;
}

CARBONATE_LINKAGE Fixed GetPortSpExtra( CGrafPtr port )
{
	return port->spExtra;
}

CARBONATE_LINKAGE short GetPortPenVisibility( CGrafPtr port )
{
	return port->pnVis;
}

CARBONATE_LINKAGE RgnHandle GetPortVisibleRegion( CGrafPtr port, RgnHandle region )
{
	MacCopyRgn( port->visRgn, region );
	
	return region;
}

CARBONATE_LINKAGE RgnHandle GetPortClipRegion( CGrafPtr port, RgnHandle region )
{
	MacCopyRgn( port->clipRgn, region );
	
	return region;
}

CARBONATE_LINKAGE PixPatHandle GetPortBackPixPat( CGrafPtr port, PixPatHandle backPattern )
{
	CopyPixPat( port->bkPixPat, backPattern );
	
	return backPattern;
}

CARBONATE_LINKAGE PixPatHandle GetPortPenPixPat( CGrafPtr port, PixPatHandle penPattern )
{
	CopyPixPat( port->pnPixPat, penPattern );
	
	return penPattern;
}

CARBONATE_LINKAGE PixPatHandle GetPortFillPixPat( CGrafPtr port, PixPatHandle fillPattern )
{
	CopyPixPat( port->fillPixPat, fillPattern );
	
	return fillPattern;
}

CARBONATE_LINKAGE Point* GetPortPenSize( CGrafPtr port, Point* penSize )
{
	*penSize = port->pnSize;
	
	return penSize;
}

CARBONATE_LINKAGE SInt32 GetPortPenMode( CGrafPtr port )
{
	return port->pnMode;
}

CARBONATE_LINKAGE Point* GetPortPenLocation( CGrafPtr port, Point* penLocation )
{
	*penLocation = port->pnLoc;
	
	return penLocation;
}

CARBONATE_LINKAGE Boolean IsPortRegionBeingDefined( CGrafPtr port )
{
	return port->rgnSave != NULL;
}

CARBONATE_LINKAGE Boolean IsPortPictureBeingDefined( CGrafPtr port )
{
	return port->picSave != NULL;
}

CARBONATE_LINKAGE Boolean IsPortPolyBeingDefined( CGrafPtr port )
{
	return port->polySave != NULL;
}

// IsPortOffscreen

CARBONATE_LINKAGE Boolean IsPortColor( CGrafPtr port )
{
	// Taken from QISA/MoreIsBetter/MoreQuickDraw.cp
	return port->portVersion < 0;
}

CARBONATE_LINKAGE Boolean IsPortVisibleRegionEmpty( CGrafPtr port )
{
	return EmptyRgn( port->visRgn );
}

CARBONATE_LINKAGE Boolean IsPortClipRegionEmpty( CGrafPtr port )
{
	return EmptyRgn( port->clipRgn );
}

// SectRegionWithPortClipRegion
// SectRegionWithPortVisibleRegion

CARBONATE_LINKAGE Handle SwapPortPicSaveHandle( CGrafPtr port, Handle newHandle )
{
	Handle oldHandle = port->picSave;
	
	port->picSave = newHandle;
	
	return oldHandle;
}

CARBONATE_LINKAGE Handle SwapPortPolySaveHandle( CGrafPtr port, Handle newHandle )
{
	Handle oldHandle = port->polySave;
	
	port->polySave = newHandle;
	
	return oldHandle;
}

CARBONATE_LINKAGE Handle SwapPortRegionSaveHandle( CGrafPtr port, Handle newHandle )
{
	Handle oldHandle = port->rgnSave;
	
	port->rgnSave = newHandle;
	
	return oldHandle;
}

// SetPortBounds
// SetPortOpColor
// SetPortGrafProcs

CARBONATE_LINKAGE void SetPortTextFont( CGrafPtr port, short txFont )
{
	port->txFont = txFont;
}

CARBONATE_LINKAGE void SetPortTextSize( CGrafPtr port, short txSize )
{
	port->txSize = txSize;
}

CARBONATE_LINKAGE void SetPortTextFace( CGrafPtr port, StyleParameter face )
{
	port->txFace = face;
}

CARBONATE_LINKAGE void SetPortTextMode( CGrafPtr port, short mode )
{
	port->txMode = mode;
}

CARBONATE_LINKAGE void SetPortVisibleRegion( CGrafPtr port, RgnHandle visRgn )
{
	MacCopyRgn( visRgn, port->visRgn );
}

CARBONATE_LINKAGE void SetPortClipRegion( CGrafPtr port, RgnHandle clipRgn )
{
	MacCopyRgn( clipRgn, port->clipRgn );
}

// SetPortPenPixPat
// SetPortFillPixPat
// SetPortBackPixPat

CARBONATE_LINKAGE void SetPortPenSize( CGrafPtr port, Point penSize )
{
	port->pnSize = penSize;
}

CARBONATE_LINKAGE void SetPortPenMode( CGrafPtr port, SInt32 penMode )
{
	port->pnMode = penMode;
}

CARBONATE_LINKAGE void SetPortFracHPenLocation( CGrafPtr port, short pnLocHFrac )
{
	port->pnLocHFrac = pnLocHFrac;
}

CARBONATE_LINKAGE Rect* GetPixBounds( PixMapHandle pixMap, Rect* bounds )
{
	*bounds = pixMap[0]->bounds;
	
	return bounds;
}

// GetPixDepth

CARBONATE_LINKAGE long GetQDGlobalsRandomSeed()
{
	return qd.randSeed;
}

CARBONATE_LINKAGE BitMap* GetQDGlobalsScreenBits( BitMap* result )
{
	*result = qd.screenBits;
	
	return result;
}

CARBONATE_LINKAGE Cursor* GetQDGlobalsArrow( Cursor* result )
{
	*result = qd.arrow;
	
	return result;
}

CARBONATE_LINKAGE Pattern* GetQDGlobalsDarkGray( Pattern* result )
{
	*result = qd.dkGray;
	
	return result;
}

CARBONATE_LINKAGE Pattern* GetQDGlobalsLightGray( Pattern* result )
{
	*result = qd.ltGray;
	
	return result;
}

CARBONATE_LINKAGE Pattern* GetQDGlobalsGray( Pattern* result )
{
	*result = qd.gray;
	
	return result;
}

CARBONATE_LINKAGE Pattern* GetQDGlobalsBlack( Pattern* result )
{
	*result = qd.black;
	
	return result;
}

CARBONATE_LINKAGE Pattern* GetQDGlobalsWhite( Pattern* result )
{
	*result = qd.white;
	
	return result;
}

CARBONATE_LINKAGE CGrafPtr GetQDGlobalsThePort()
{
	return reinterpret_cast< CGrafPtr >( qd.thePort );
}

CARBONATE_LINKAGE void SetQDGlobalsRandomSeed( long randomSeed )
{
	qd.randSeed = randomSeed;
}

CARBONATE_LINKAGE void SetQDGlobalsArrow( const Cursor* arrow )
{
	qd.arrow = *arrow;
}

CARBONATE_LINKAGE Rect* GetRegionBounds( RgnHandle region, Rect* bounds )
{
	*bounds = region[0]->rgnBBox;
	
	return bounds;
}

// IsRegionRectangular

CARBONATE_LINKAGE CGrafPtr CreateNewPort()
{
	Ptr portMem = NewPtr( sizeof (CGrafPort) );
	
	if ( portMem == NULL )  return NULL;
	
	CGrafPtr port = reinterpret_cast< CGrafPtr >( portMem );
	
	OpenCPort( port );
	
	return port;
}

CARBONATE_LINKAGE void DisposePort( CGrafPtr port )
{
	CloseCPort( port );
	
	DisposePtr( reinterpret_cast< Ptr >( port ) );
}

// SetQDError

#undef CARBONATE_LINKAGE

#endif

#endif

