/*	==============
 *	GraphicView.hh
 *	==============
 */

#ifndef PEDESTAL_GRAPHICVIEW_HH
#define PEDESTAL_GRAPHICVIEW_HH

// Pedestal
#include "Pedestal/View.hh"


namespace Pedestal
{
	
	template < class Graphic >
	class GraphicView : public View
	{
		private:
			Rect bounds;
			Graphic graphic;
		
		public:
			typedef typename Graphic::Initializer Initializer;
			
			GraphicView( const Rect& bounds, Initializer init = Initializer() )
			:
				bounds ( bounds ),
				graphic( init   )
			{}
			
			Graphic const& Get() const  { return graphic; }
			Graphic      & Get()        { return graphic; }
			
			const Rect& Bounds() const  { return bounds; }
			
			void Update()  { graphic.Plot( bounds ); }
	};
	
}

#endif

