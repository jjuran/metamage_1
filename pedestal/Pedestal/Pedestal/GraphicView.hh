/*	==============
 *	GraphicView.hh
 *	==============
 */

#pragma once

// Pedestal
#include "Pedestal/View.hh"


namespace Pedestal
{
	
	namespace N = Nitrogen;
	
	template < class Graphic >
	class GraphicView : public View
	{
		private:
			Rect bounds;
			Graphic graphic;
		
		public:
			typedef typename Graphic::Initializer Initializer;
			
			GraphicView( const Rect& bounds, Initializer init )
			:
				bounds ( bounds ),
				graphic( init   )
			{}
			
			Graphic& Get() const  { return graphic; }
			
			const Rect& Bounds() const  { return bounds; }
			
			void Update()  { graphic.Plot( bounds ); }
	};
	
}

