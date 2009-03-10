/*	========
 *	Frame.cc
 *	========
 */

#include "Pedestal/Frame.hh"


namespace Pedestal
{
	
	namespace N = Nitrogen;
	
	
	static inline Rect GetAperture( const Rect& bounds, short padding )
	{
		return N::InsetRect( bounds, padding, padding );
	}
	
	Rect Frame::ApertureFromBounds( const Rect& bounds )
	{
		return GetAperture( bounds, Padding() );
	}
	
	void Frame::Draw( const Rect& bounds, bool erasing )
	{
		short padding = Padding();
		
		if ( erasing )
		{
			Rect top    = bounds;
			Rect bottom = bounds;
			
			Rect left  = bounds;
			Rect right = bounds;
			
			top.bottom = top.top   + padding;
			left.right = left.left + padding;
			
			bottom.top = bottom.bottom - padding;
			right.left = right.right   - padding;
			
			N::EraseRect( top    );
			N::EraseRect( left   );
			N::EraseRect( right  );
			N::EraseRect( bottom );
		}
		
		Subview().Draw( GetAperture( bounds, padding ), erasing );
	}
	
}

