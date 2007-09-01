/*	=============
 *	GWorldView.hh
 *	=============
 */

#ifndef PEDESTAL_GWORLDVIEW_HH
#define PEDESTAL_GWORLDVIEW_HH

// Nitrogen
#include "Nitrogen/QDOffScreen.h"

// Pedestal
#include "Pedestal/View.hh"


namespace Pedestal
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	
	class GWorldView : public View
	{
		private:
			Rect bounds;
			NN::Owned< N::GWorldPtr > myGWorld;
		
		public:
			GWorldView( const Rect& bounds, Initializer );
			
			N::GWorldPtr Get() const  { return myGWorld.Get(); }
			
			const Rect& Bounds() const  { return bounds; }
			
			void Update();
	};
	
}

#endif

