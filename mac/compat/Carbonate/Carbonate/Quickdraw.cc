// Carbonate/Quickdraw.cc


#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif

#if !TARGET_API_MAC_CARBON

#if ACCESSOR_CALLS_ARE_FUNCTIONS
// Compile the Carbon accessors as extern pascal functions.
#define CARBONATE_LINKAGE pascal
#include "Carbonate/Quickdraw.hh"
#endif

// These functions are always declared in the headers and are always extern.

pascal OSErr LockPortBits( GrafPtr port )
{
	return noErr;
}

pascal OSErr UnlockPortBits( GrafPtr port )
{
	return noErr;
}

// QDRegionToRects

pascal Boolean QDSwapPort( CGrafPtr newCPort, CGrafPtr* oldCPortPtr )
{
	GrafPtr oldPort;
	GetPort( &oldPort );
	
	if ( oldCPortPtr != NULL )
	{
		*oldCPortPtr = reinterpret_cast< CGrafPtr >( oldPort );
	}
	
	GrafPtr newPort = reinterpret_cast< GrafPtr >( newCPort );
	
	SetPort( newPort );
	
	return oldPort != newPort;
}

// HandleToRgn
// RgnToHandle
// StdOpcode

pascal Boolean IsValidPort( CGrafPtr port )
{
	// FIXME:  What logic should go here?
	bool valid = port != NULL;
	
	return valid;
}

pascal Point* QDLocalToGlobalPoint( CGrafPtr port, Point* point )
{
	CGrafPtr saved;
	bool swapped = QDSwapPort( port, &saved );
	
	LocalToGlobal( point );
	
	if ( swapped )
	{
		(void) QDSwapPort( saved, NULL );
	}
	
	return point;
}

pascal Point* QDGlobalToLocalPoint( CGrafPtr port, Point* point )
{
	CGrafPtr saved;
	bool swapped = QDSwapPort( port, &saved );
	
	GlobalToLocal( point );
	
	if ( swapped )
	{
		(void) QDSwapPort( saved, NULL );
	}
	
	return point;
}

pascal Rect* QDLocalToGlobalRect( CGrafPtr port, Rect* rect )
{
	CGrafPtr saved;
	bool swapped = QDSwapPort( port, &saved );
	
	Point* points = reinterpret_cast< Point* >( rect );
	
	LocalToGlobal( &points[0] );
	LocalToGlobal( &points[1] );
	
	if ( swapped )
	{
		(void) QDSwapPort( saved, NULL );
	}
	
	return rect;
}

pascal Rect* QDGlobalToLocalRect( CGrafPtr port, Rect* rect )
{
	CGrafPtr saved;
	bool swapped = QDSwapPort( port, &saved );
	
	Point* points = reinterpret_cast< Point* >( rect );
	
	GlobalToLocal( &points[0] );
	GlobalToLocal( &points[1] );
	
	if ( swapped )
	{
		(void) QDSwapPort( saved, NULL );
	}
	
	return rect;
}

// QDLocalToGlobalRegion
// QDGlobalToLocalRegion

pascal Boolean QDIsPortBuffered( CGrafPtr port )
{
	return false;
}

pascal Boolean QDIsPortBufferDirty( CGrafPtr port )
{
	return false;
}

pascal void QDFlushPortBuffer( CGrafPtr port, RgnHandle region )
{
	// do nothing
}

// QDGetDirtyRegion
// QDSetDirtyRegion
// QDAddRectToDirtyRegion
// QDAddRegionToDirtyRegion
// QDSaveRegionBits
// QDRestoreRegionBits
// QDDisposeRegionBits

#endif

