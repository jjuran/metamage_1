/*	=================
 *	Pedestal/Frame.hh
 *	=================
 */

#ifndef PEDESTAL_FRAME_HH
#define PEDESTAL_FRAME_HH

// Pedestal
#include "Pedestal/View.hh"


namespace Pedestal
{
	
	class Frame : public View
	{
		public:
			Frame( const Rect& bounds )
			{
			}
			
			virtual short Margin() const = 0;
			
			virtual View& Subview() = 0;
			
			void Draw( const Rect& bounds );
	};
	
}

#endif

