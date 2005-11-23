/*	====================
 *	SolidColorGraphic.cc
 *	====================
 */

#include "Pedestal/SolidColorGraphic.hh"

// Nitrogen
#include "Nitrogen/QuickDraw.h"

// Nitrogen Extras / Utilities
#include "Utilities/Saved.h"


namespace Pedestal
{
	
	namespace N = Nitrogen;
	
	SolidColorGraphic::SolidColorGraphic( const RGBColor& color )
	:
	  	color( color )
	{
	}
	
	void SolidColorGraphic::Plot( const Rect& area )
	{
		N::Saved< N::RGBForeColor_Value > saved;
		
		N::RGBForeColor( color );
		
		N::PaintRect( area );
	}
	
}

