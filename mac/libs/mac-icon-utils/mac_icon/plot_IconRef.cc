/*
	plot_IconRef.cc
	---------------
*/

#include "mac_icon/plot_IconRef.hh"


namespace mac  {
namespace icon {

void plot_IconRef( CGContextRef context, CGRect bounds, IconRef icon )
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
}
