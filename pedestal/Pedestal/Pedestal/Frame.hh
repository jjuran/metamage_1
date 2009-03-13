/*	=================
 *	Pedestal/Frame.hh
 *	=================
 */

#ifndef PEDESTAL_FRAME_HH
#define PEDESTAL_FRAME_HH

// Pedestal
#include "Pedestal/Superview.hh"


namespace Pedestal
{
	
	class Frame : public Superview
	{
		public:
			virtual short Padding() const = 0;
			
			virtual short OutlineWidth() const = 0;
			virtual short OutlineOffset() const = 0;
			
			virtual Rect Margin( const Rect& bounds ) const = 0;
			
			Rect ApertureFromBounds( const Rect& bounds );
			
			void Draw( const Rect& bounds, bool erasing );
	};
	
}

#endif

