/*	===========
 *	Scroller.cc
 *	===========
 */

#include "Pedestal/Scroller.hh"

// Standard C++
#include <algorithm>

// Nitrogen
#include "Nitrogen/ControlDefinitions.h"


namespace Pedestal
{
	
	ClickableScroller* ClickableScroller::gCurrentScroller = NULL;
	
	short ActualScrollbarLength( short viewLength, bool shortened )
	{
		return 
			ScrollbarOverlap() 
			+ viewLength 
			- (shortened ? ScrollbarProfile() : 0) 
			+ ScrollbarOverlap();
	}
	
	Rect VerticalScrollbarBounds( UInt16 width, UInt16 height, bool shortened )
	{
		return N::OffsetRect( N::SetRect( -ScrollbarThickness(),
		                                  0,
		                                  0,
		                                  ActualScrollbarLength( height, shortened ) ),
		                      width + ScrollbarOverlap(),
		                      -ScrollbarOverlap() );
	}
	
	Rect HorizontalScrollbarBounds( UInt16 width, UInt16 height, bool shortened )
	{
		return N::OffsetRect( N::SetRect( 0,
		                                  -ScrollbarThickness(),
		                                  ActualScrollbarLength( width, shortened ),
		                                  0 ),
		                      -ScrollbarOverlap(),
		                      height + ScrollbarOverlap() );
	}
	
	Rect Aperture( UInt16 width, UInt16 height, bool vertical, bool horizontal )
	{
		return N::SetRect( 0,
		                   0,
		                   width  - ( vertical   ? ScrollbarProfile() : 0),
		                   height - ( horizontal ? ScrollbarProfile() : 0) );
	}
	
	Point ScrollbarMaxima( Point scrollableRange, Point viewableRange, Point scrollPosition )
	{
		return N::SetPt
		(
			std::max( scrollableRange.h - viewableRange.h, scrollPosition.h - 0 ), 
			std::max( scrollableRange.v - viewableRange.v, scrollPosition.v - 0 )
		);
		
	}
	
	short FigureScrollDistance( N::ControlPartCode part, short pageDistance )
	{
		short scrollDistance = 0;
		
		switch ( part )
		{
			case kControlUpButtonPart:
			case kControlDownButtonPart:
				scrollDistance = 1;
				break;
			
			case kControlPageUpPart:
			case kControlPageDownPart:
				scrollDistance = pageDistance;
				break;
		}
		
		if ( part == N::kControlUpButtonPart  ||  part == N::kControlPageUpPart )
		{
			scrollDistance = -scrollDistance;
		}
		
		return scrollDistance;
	}
	
	static short SetClippedControlValue( ControlRef control, short value )
	{
		value = std::max( value, N::GetControlMinimum( control ) );
		value = std::min( value, N::GetControlMaximum( control ) );
		
		N::SetControlValue( control, value );
		
		return value;
	}
	
	short SetControlValueFromClippedDelta( ControlRef control, short delta )
	{
		short oldValue = N::GetControlValue( control );
		
		short value = SetClippedControlValue( control, oldValue + delta );
		
		return value - oldValue;
	}
	
}

