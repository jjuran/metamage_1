/*	========
 *	Frame.cc
 *	========
 */

#include "Pedestal/Frame.hh"

// Mac OS X
#ifdef __APPLE__
#include <Carbon/Carbon.h>
#endif

// Mac OS
#ifndef __EVENTS__
#include <Events.h>
#endif
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif

// missing-macos
#ifdef MAC_OS_X_VERSION_10_7
#ifndef MISSING_QUICKDRAW_H
#include "missing/Quickdraw.h"
#endif
#endif


namespace Pedestal
{
	
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
		Rect aperture = box;
		
		InsetRect( &aperture, padding, padding );
		
		return aperture;
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
		
		Point pt = event.where;
		
		GlobalToLocal( &pt );
		
		return PtInRect( pt, &box );
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
			
			top.bottom = top.top   + padding;
			left.right = left.left + padding;
			
			bottom.top = bottom.bottom - padding;
			right.left = right.right   - padding;
			
			if ( erasing )
			{
				top.bottom += margin.top;
				left.right += margin.left;
				
				bottom.top -= margin.bottom;
				right.left -= margin.right;
			}
			
			EraseRect( &top    );
			EraseRect( &left   );
			EraseRect( &right  );
			EraseRect( &bottom );
		}
		
		Subview().Draw( aperture, erasing );
		
		if ( const short width = OutlineWidth() )
		{
			const short offset = OutlineOffset();
			
			const short outset = width + offset;
			
			Rect outline_bounds = box;
			
			InsetRect( &outline_bounds, -outset, -outset );
			
			PenSize( width, width );
			
			if ( const short diameter = OutlineCurvature() )
			{
				FrameRoundRect( &outline_bounds, diameter, diameter );
			}
			else
			{
				FrameRect( &outline_bounds );
			}
			
			PenNormal();
		}
	}
	
}
