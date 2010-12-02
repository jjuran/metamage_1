/*	=================
 *	Pedestal/Frame.hh
 *	=================
 */

#ifndef PEDESTAL_FRAME_HH
#define PEDESTAL_FRAME_HH

// Pedestal
#ifndef PEDESTAL_SUPERVIEW_HH
#include "Pedestal/Superview.hh"
#endif


namespace Pedestal
{
	
	class Frame : public Superview
	{
		private:
			Rect itsSavedBounds;
		
		public:
			virtual short Padding() const = 0;
			
			virtual short OutlineWidth() const = 0;
			virtual short OutlineOffset() const = 0;
			virtual short OutlineCurvature() const = 0;
			
			virtual Rect Margin( const Rect& bounds ) const = 0;
			
			Rect ApertureFromBounds( const Rect& bounds );
			
			bool HitTest( const EventRecord& event );
			
			void Draw( const Rect& bounds, bool erasing );
	};
	
}

#endif

