/*
	HIViewPlotIconRef.cc
	--------------------
*/

#include "Pedestal/HIViewPlotIconRef.hh"

// Nitrogen
#include "Nitrogen/Icons.hh"


namespace Pedestal
{
	
	namespace N = Nitrogen;
	
	
	void HIViewPlotIconRef( CGContextRef context, CGRect bounds, IconRef icon )
	{
		CGContextSaveGState( context );
		
		CGContextTranslateCTM( context, bounds.origin.x, bounds.origin.y );
		
		bounds.origin = CGPointMake( 0, 0 );
		
		CGContextTranslateCTM( context, 0, bounds.size.height );
		CGContextScaleCTM    ( context, 1, -1 );
		
		N::PlotIconRefInContext( context, bounds, icon );
		
		CGContextRestoreGState( context );
	}
	
}
