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

#if PRAGMA_ONCE
#pragma once
#endif

#if PRAGMA_ONCE
#pragma once
#endif


namespace Pedestal
{
	
	template < class Front, class Back >
	class FrontBackView : public View
	{
		private:
			Rect bounds;
			
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
			
			const Rect& Bounds() const  { return bounds; }
			
			void Update()
			{
				back .Update();
				front.Update();
			}
	};
	
}

#endif

