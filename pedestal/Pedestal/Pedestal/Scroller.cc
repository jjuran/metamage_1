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
	
	namespace N = Nitrogen;
	
	
	static inline long Gestalt_AppearanceManager()
	{
		return N::Gestalt( N::Gestalt_Selector( gestaltAppearanceAttr ), 0 );
	}
	
#if !TARGET_API_MAC_CARBON
	
	bool AppearanceManagerExists()
	{
		static bool exists = Gestalt_AppearanceManager() != 0;
		
		return exists;
	}
	
#endif
	
	
	ClickableScroller* ClickableScroller::gCurrentScroller = NULL;
	
	
	Point ComputeScrollbarMaxima( const ScrollableBase& scrolledView )
	{
		Point scrollRange = scrolledView.ScrollableRange();
		Point viewRange   = scrolledView.ViewableRange();
		
		Point scrollPos   = scrolledView.ScrollPosition();
		
		Point maxima = ScrollbarMaxima( scrollRange, viewRange, scrollPos );
		
		return maxima;
	}
	
	short ActualScrollbarLength( short viewLength, bool shortened )
	{
		return 
			ScrollbarOverlap() 
			+ viewLength 
			- (shortened ? ScrollbarProfile() : 0) 
			+ ScrollbarOverlap();
	}
	
	Rect VerticalScrollbarBounds( short width, short height, bool shortened )
	{
		return N::OffsetRect( N::SetRect( -ScrollbarThickness(),
		                                  0,
		                                  0,
		                                  ActualScrollbarLength( height, shortened ) ),
		                      width + ScrollbarOverlap(),
		                      -ScrollbarOverlap() );
	}
	
	Rect HorizontalScrollbarBounds( short width, short height, bool shortened )
	{
		return N::OffsetRect( N::SetRect( 0,
		                                  -ScrollbarThickness(),
		                                  ActualScrollbarLength( width, shortened ),
		                                  0 ),
		                      -ScrollbarOverlap(),
		                      height + ScrollbarOverlap() );
	}
	
	Point ScrollDimensions( short width, short height, bool vertical, bool horizontal )
	{
		return N::SetPt( width  - ( vertical   ? ScrollbarProfile() : 0 ),
		                 height - ( horizontal ? ScrollbarProfile() : 0 ) );
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
	
	void ScrollerBase::Idle( const EventRecord& event )
	{
		// Intersect the clip region with the scrollview bounds,
		// so the scrollview doesn't overpaint the scroll bars.
		Nucleus::Saved< Nitrogen::Clip_Value > savedClip( Nitrogen::SectRgn( Nitrogen::GetClip(),
		                                                                     Nitrogen::RectRgn( GetSubView().Bounds() ) ) );
		
		GetSubView().Idle( event );
	}
	
	void ScrollerBase::MouseDown( const EventRecord& event )
	{
		if ( Nitrogen::PtInRect( Nitrogen::GlobalToLocal( event.where ), GetSubView().Bounds() ) )
		{
			gCurrentScroller = this;
			
			GetSubView().MouseDown( event );
			
			gCurrentScroller = NULL;
		}
	}
	
	void ScrollerBase::Draw( const Rect& bounds )
	{
		const Rect& subviewBounds = GetSubView().Bounds();
		
		// Intersect the clip region with the scrollview bounds,
		// so the scrollview doesn't overpaint the scroll bars.
		Nucleus::Saved< Nitrogen::Clip_Value > savedClip( Nitrogen::SectRgn( Nitrogen::GetClip(),
		                                                                     Nitrogen::RectRgn( subviewBounds ) ) );
		
		GetSubView().Draw( subviewBounds );
	}
	
	bool ScrollerBase::SetCursor( Point location, RgnHandle mouseRgn )
	{
		if ( Nitrogen::PtInRect( location, GetSubView().Bounds() ) )
		{
			return GetSubView().SetCursor( location, mouseRgn );
		}
		
		return false;
	}
	
}

