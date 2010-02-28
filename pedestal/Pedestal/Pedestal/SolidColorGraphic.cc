/*	====================
 *	SolidColorGraphic.cc
 *	====================
 */

#include "Pedestal/SolidColorGraphic.hh"

// Nucleus
#include "Nucleus/Saved.h"

// Nitrogen
#include "Nitrogen/Quickdraw.hh"


namespace Pedestal
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	
	SolidColorGraphic::SolidColorGraphic( const RGBColor& color )
	:
	  	color( color )
	{
	}
	
	void SolidColorGraphic::Plot( const Rect& area )
	{
		NN::Saved< N::RGBForeColor_Value > saved;
		
		N::RGBForeColor( color );
		
		N::PaintRect( area );
	}
	
}

