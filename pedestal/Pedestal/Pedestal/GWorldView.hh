/*	=============
 *	GWorldView.hh
 *	=============
 */

#pragma once

// Nitrogen
#include "Nitrogen/QDOffScreen.h"

// Pedestal
#include "Pedestal/View.hh"


namespace Pedestal
{
	
	namespace N = Nitrogen;
	
	class GWorldView
	{
		private:
			Rect bounds;
			N::Owned< N::GWorldPtr, N::GWorldDisposer > myGWorld;
		
		public:
			GWorldView( const Rect& bounds );
			
			N::GWorldPtr Get() const  { return myGWorld.Get(); }
			
			const Rect& Bounds() const  { return bounds; }
			
			void Update();
	};
	
}

