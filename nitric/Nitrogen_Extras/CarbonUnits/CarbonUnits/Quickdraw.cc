// CarbonUnits/Quickdraw.cc


#ifndef __QUICKDRAW__
	// ACCESSOR_CALLS_ARE_FUNCTIONS is permitted to be false if Quickdraw.h has
	// already been included.  Otherwise, it must be true.
	#if !defined(ACCESSOR_CALLS_ARE_FUNCTIONS) || !ACCESSOR_CALLS_ARE_FUNCTIONS
		#error Configuration error:  ACCESSOR_CALLS_ARE_FUNCTIONS must be true
	#endif
	#include <Quickdraw.h>
#endif

#if TARGET_API_MAC_CARBON
#error Configuration error:  This file is for classic only
#endif

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

#if TARGET_CPU_68K

pascal RGBColor* GetPortForeColor( CGrafPtr port, RGBColor* color )
{
	*color = port->rgbFgColor;
	
	return color;
}

pascal RGBColor* GetPortBackColor( CGrafPtr port, RGBColor* color )
{
	*color = port->rgbBkColor;
	
	return color;
}

pascal RgnHandle GetPortVisibleRegion( CGrafPtr cport, RgnHandle region )
{
	GrafPtr port = reinterpret_cast< GrafPtr >( cport );
	
	MacCopyRgn( port->visRgn, region );
	
	return region;
}

pascal Boolean IsPortColor( CGrafPtr port )
{
	// Taken from QISA/MoreIsBetter/MoreQuickDraw.cp
	return port->portVersion < 0;
}

#endif

