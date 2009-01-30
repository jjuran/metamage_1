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
			virtual short Margin() const = 0;
			
			void Draw( const Rect& bounds );
			
			void Resize( short width, short height );
	};
	
}

#endif

