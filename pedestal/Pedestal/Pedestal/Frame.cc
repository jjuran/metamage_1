/*	========
 *	Frame.cc
 *	========
 */

#include "Pedestal/Frame.hh"

// Nitrogen
#include "Nitrogen/QuickDraw.h"


namespace Pedestal
{
	
	namespace N = Nitrogen;
	
	
	static inline Rect GetPaddingBox( const Rect& bounds, const Rect& margin )
	{
		Rect box = bounds;
		
		box.left   += margin.left;
		box.top    += margin.top;
		box.right  -= margin.right;
		box.bottom -= margin.bottom;
		
		return box;
	}
	
	static inline Rect GetApertureFromBox( const Rect& box, short padding )
	{
		return N::InsetRect( box, padding, padding );
	}
	
	static inline Rect GetAperture( const Rect& bounds, const Rect& margin, short padding )
	{
		return GetApertureFromBox( GetPaddingBox( bounds, margin ), padding );
	}
	
	Rect Frame::ApertureFromBounds( const Rect& bounds )
	{
		return GetAperture( bounds, Margin( bounds ), Padding() );
	}
	
	bool Frame::HitTest( const EventRecord& event )
	{
		const Rect margin = Margin( itsSavedBounds );
		
		const Rect box = GetPaddingBox( itsSavedBounds, margin );
		
		return N::PtInRect( N::GlobalToLocal( event.where ), box );
	}
	
	void Frame::Draw( const Rect& bounds, bool erasing )
	{
		itsSavedBounds = bounds;
		
		const Rect margin = Margin( bounds );
		
		const short padding = Padding();
		
		const Rect box = GetPaddingBox( bounds, margin );
		
		const Rect aperture = GetApertureFromBox( box, padding );
		
		// erasing is true if we should erase the margin.
		// We always erase the padding.
		
		if ( erasing  ||  padding > 0 )
		{
			const Rect& frame = erasing ? bounds : box;
			
			Rect top    = frame;
			Rect bottom = frame;
			
			Rect left  = frame;
			Rect right = frame;
			
			const Rect offsets = erasing ? margin : N::SetRect( 0, 0, 0, 0 );
			
			top.bottom = top.top   + offsets.top  + padding;
			left.right = left.left + offsets.left + padding;
			
			bottom.top = bottom.bottom - offsets.bottom - padding;
			right.left = right.right   - offsets.right  - padding;
			
			N::EraseRect( top    );
			N::EraseRect( left   );
			N::EraseRect( right  );
			N::EraseRect( bottom );
		}
		
		Subview().Draw( aperture, erasing );
		
		if ( const short width = OutlineWidth() )
		{
			const short offset = OutlineOffset();
			
			const short outset = width + offset;
			
			const Rect outline_bounds = N::InsetRect( box, -outset, -outset );
			
			N::PenSize( width, width );
			
			if ( const short diameter = OutlineCurvature() )
			{
				::FrameRoundRect( &outline_bounds, diameter, diameter );
			}
			else
			{
				N::FrameRect( outline_bounds );
			}
			
			N::PenNormal();
		}
	}
	
}

