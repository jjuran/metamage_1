/*	===============
 *	RectangleMath.h
 *	===============
 */

#ifndef RECTANGLEMATH_H
#define RECTANGLEMATH_H

// Nitrogen
#ifndef NITROGEN_QUICKDRAW_H
#include "Nitrogen/QuickDraw.h"
#endif


namespace NitrogenExtras
{
	
	namespace N = Nitrogen;
	
	using namespace N::Operators;
	
	inline int RectWidth( const Rect& rect )
	{
		return rect.right - rect.left;
	}
	
	inline int RectHeight( const Rect& rect )
	{
		return rect.bottom - rect.top;
	}
	
	inline bool IsEmptyRect( const Rect& rect )
	{
		return RectWidth ( rect ) <= 0
		    || RectHeight( rect ) <= 0;
	}
	
	inline Point RectPosition( const Rect& rect )
	{
		return N::SetPt( rect.left, 
		                 rect.top );
	}
	
	inline Point RectSize(const Rect& rect)
	{
		return N::SetPt( RectWidth ( rect ), 
		                 RectHeight( rect ) );
	}
	
	inline Point RectCenterOffset( const Rect& rect )
	{
		return RectSize( rect ) / 2;
	}
	
	inline short RectHMidpoint( const Rect& rect )
	{
		return (rect.left + rect.right) / 2;
	}
	
	inline short RectVMidpoint( const Rect& rect )
	{
		return (rect.top + rect.bottom) / 2;
	}
	
	inline Point RectCenter( const Rect& rect )
	{
		return N::SetPt( RectHMidpoint( rect ), 
		                 RectVMidpoint( rect ) );
	}
	
	inline Rect MakeRect( Point loc, Point size )
	{
		return N::SetRect( loc.h, 
		                   loc.v, 
		                   loc.h + size.h, 
		                   loc.v + size.v );
	}
	
	inline Rect NormalRect( const Rect& r )
	{
		return N::SetRect( 0, 
		                   0, 
		                   r.right  - r.left, 
		                   r.bottom - r.top );
	}
	
	
	// CenteredMargin
	// 
	// Calculates the margin for centering one length within another.
	// For example, if you want to center a 700-pixel-wide window (the object)
	// within an 800-pixel-wide screen (the frame), then CenteredMargin(800, 700) 
	// yields 50, the margin on each side.
	
	inline int CenteredMargin( int frameLength, int objectLength )
	{
		return (frameLength - objectLength) / 2;
	}
	
	
	// CenteredRectOffset
	// 
	// Calculates the 2D offset of an object centered within a frame -- that is, 
	// the difference between the top-left corner of the object and the top-left
	// corner of the frame.
	
	inline Point CenteredRectOffset( Point frameSize, Point objectSize )
	{
		return N::SetPt( CenteredMargin( frameSize.h, objectSize.h ), 
		                 CenteredMargin( frameSize.v, objectSize.v ) );
	}
	
	
	// CenteredRect
	// 
	// Given a frame rectangle and the dimensions of an object centered within it, 
	// calculates the rectangle of that object.
	
	inline Rect CenteredRect( const Rect& frame, Point size )
	{
		return MakeRect( RectPosition( frame ) + CenteredRectOffset( RectSize( frame ), size ), 
		                 size );
	}
	
	
}

#endif

