/*	====================
 *	SolidColorGraphic.hh
 *	====================
 */

#pragma once

// Pedestal
#include "Pedestal/Graphic.hh"


namespace Pedestal
{
	
	class SolidColorGraphic : public Graphic
	{
		private:
			RGBColor color;
		
		public:
			typedef RGBColor Initializer;
			
			SolidColorGraphic( const RGBColor& color );
			
			RGBColor Get() const  { return color; }
			
			void Plot( const Rect& area );
	};
	
}

