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
			Graphic graphic;
		
		public:
			typedef typename Graphic::Initializer Initializer;
			
			GraphicView( Initializer init = Initializer() )
			:
				graphic( init )
			{
			}
			
			Graphic const& Get() const  { return graphic; }
			Graphic      & Get()        { return graphic; }
			
			void Draw( const Rect& bounds, bool erasing )  { graphic.Plot( bounds ); }
	};
	
}

#endif

