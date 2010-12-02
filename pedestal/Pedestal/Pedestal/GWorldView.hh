/*	=============
 *	GWorldView.hh
 *	=============
 */

#ifndef PEDESTAL_GWORLDVIEW_HH
#define PEDESTAL_GWORLDVIEW_HH

// Nitrogen
#ifndef NITROGEN_QDOFFSCREEN_HH
#include "Nitrogen/QDOffScreen.hh"
#endif

// Pedestal
#ifndef PEDESTAL_VIEW_HH
#include "Pedestal/View.hh"
#endif


namespace Pedestal
{
	
	class GWorldView : public View
	{
		private:
			Rect                         itsBounds;
			nucleus::owned< GWorldPtr >  itsGWorld;
		
		public:
			GWorldView( const Rect& bounds );
			
			GWorldPtr Get() const  { return itsGWorld.get(); }
			
			const Rect& Bounds() const  { return itsBounds; }
			
			void Erase( const Rect& bounds );
			
			void Resize( short width, short height );
			
			void Draw( const Rect& bounds, bool erasing );
	};
	
}

#endif

