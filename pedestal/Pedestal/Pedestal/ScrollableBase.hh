/*	=================
 *	ScrollableBase.hh
 *	=================
 */

#ifndef PEDESTAL_SCROLLABLEBASE_HH
#define PEDESTAL_SCROLLABLEBASE_HH

// Pedestal
#include "Pedestal/View.hh"


namespace Pedestal
{
	
	class ScrollableBase : public View
	{
		public:
			virtual Rect Bounds() const = 0;
			
			virtual Point ViewableRange() const = 0;
			virtual Point ScrollableRange() const = 0;
			virtual Point ScrollPosition() const = 0;
			
			virtual void Scroll( short dh, short dv, bool updateNow ) = 0;
	};
	
}

#endif

