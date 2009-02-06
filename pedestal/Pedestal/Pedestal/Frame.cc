/*	========
 *	Frame.cc
 *	========
 */

#include "Pedestal/Frame.hh"


namespace Pedestal
{
	
	namespace N = Nitrogen;
	
	
	static inline Rect GetAperture( const Rect& bounds, short margin )
	{
		return N::InsetRect( bounds, margin, margin );
	}
	
	Rect Frame::ApertureFromBounds( const Rect& bounds )
	{
		return GetAperture( bounds, Margin() );
	}
	
	void Frame::Draw( const Rect& bounds )
	{
		short margin = Margin();
		
		Rect top    = bounds;
		Rect bottom = bounds;
		
		Rect left  = bounds;
		Rect right = bounds;
		
		top.bottom = top.top   + margin;
		left.right = left.left + margin;
		
		bottom.top = bottom.bottom - margin;
		right.left = right.right   - margin;
		
		N::EraseRect( top    );
		N::EraseRect( left   );
		N::EraseRect( right  );
		N::EraseRect( bottom );
		
		Subview().Draw( GetAperture( bounds, margin ) );
	}
	
	void Frame::Resize( short width, short height )
	{
		short margin = Margin();
		
		Subview().Resize( width  - 2 * margin,
		                  height - 2 * margin );
	}
	
}

