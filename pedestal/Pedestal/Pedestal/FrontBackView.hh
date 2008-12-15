/*	================
 *	FrontBackView.hh
 *	================
 */

#ifndef PEDESTAL_FRONTBACKVIEW_HH
#define PEDESTAL_FRONTBACKVIEW_HH

// Standard C++
#include <utility>

// Pedestal
#include "Pedestal/View.hh"


namespace Pedestal
{
	
	template < class Front, class Back >
	class FrontBackView : public View
	{
		private:
			Front front;
			Back  back;
		
		public:
			typedef std::pair< typename Front::Initializer,
			                   typename Back ::Initializer > Initializer;
			
			FrontBackView( const Rect& bounds, Initializer init )
			:
				front( bounds, init.first  ),
				back ( bounds, init.second )
			{}
			
			Front& GetFront() const  { return front; }
			Back & GetBack () const  { return back;  }
			
			void Draw( const Rect& bounds )
			{
				back .Draw( bounds );
				front.Draw( bounds );
			}
	};
	
}

#endif

