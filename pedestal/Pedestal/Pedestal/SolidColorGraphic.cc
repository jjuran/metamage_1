/*	====================
 *	SolidColorGraphic.cc
 *	====================
 */

#include "Pedestal/SolidColorGraphic.hh"

// nucleus
#include "nucleus/saved.hh"

// Nitrogen
#include "Nitrogen/Quickdraw.hh"


namespace Pedestal
{
	
	namespace n = nucleus;
	namespace N = Nitrogen;
	
	SolidColorGraphic::SolidColorGraphic( const RGBColor& color )
	:
	  	color( color )
	{
	}
	
	void SolidColorGraphic::Plot( const Rect& area )
	{
		n::saved< N::RGBForeColor_Setting > saved;
		
		N::RGBForeColor( color );
		
		N::PaintRect( area );
	}
	
}

