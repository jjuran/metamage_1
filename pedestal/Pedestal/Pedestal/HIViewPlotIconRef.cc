/*
	HIViewPlotIconRef.cc
	--------------------
*/

#include "Pedestal/HIViewPlotIconRef.hh"

// Mac OS
#ifndef __ICONS__
#include <Icons.h>
#endif


namespace Pedestal
{
	
	void HIViewPlotIconRef( CGContextRef context, CGRect bounds, IconRef icon )
	{
		CGContextSaveGState( context );
		
		CGContextTranslateCTM( context, bounds.origin.x, bounds.origin.y );
		
		bounds.origin = CGPointMake( 0, 0 );
		
		CGContextTranslateCTM( context, 0, bounds.size.height );
		CGContextScaleCTM    ( context, 1, -1 );
		
		PlotIconRefInContext( context, &bounds, 0, 0, NULL, 0, icon );
		
		CGContextRestoreGState( context );
	}
	
}
