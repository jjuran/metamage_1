/*	====================
 *	SolidColorGraphic.cc
 *	====================
 */

#include "Pedestal/SolidColorGraphic.hh"

// Nitrogen
#include "Nitrogen/QuickDraw.h"
#include "Nitrogen/Scoped.h"


namespace Pedestal
{
	
	namespace N = Nitrogen;
	
	SolidColorGraphic::SolidColorGraphic( const RGBColor& color )
	:
	  	color ( color  )
	{
	}
	
	void SolidColorGraphic::Plot( const Rect& area )
	{
		N::Scoped< N::The_RGBForeColor > colorScope;
		
		N::RGBForeColor( color );
		
		N::PaintRect( area );
	}
	
}

