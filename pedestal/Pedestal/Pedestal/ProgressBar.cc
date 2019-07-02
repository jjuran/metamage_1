/*	==============
 *	ProgressBar.cc
 *	==============
 */

#include "Pedestal/ProgressBar.hh"

// missing-macos
#ifdef MAC_OS_X_VERSION_10_7
#ifndef MISSING_QUICKDRAW_H
#include "missing/Quickdraw.h"
#endif
#endif

// MacFeatures
#include "MacFeatures/ColorQuickdraw.hh"


namespace Pedestal
{
	
	enum
	{
		kStdProgressBarHeight = 12, 
		kSideMargin = 12
	};
	
	static RGBColor gBlack = { 0, 0, 0 };
	
	static RGBColor gDarkGrey = { 0x4444, 0x4444, 0x4444 };
	static RGBColor gSkyBlue  = { 0xcccc, 0xcccc, 0xffff };
	
	
	static void PaintRect_In_Color( const Rect& bounds, const RGBColor& color )
	{
		RGBForeColor( &color );
		
		PaintRect( &bounds );
		
		RGBForeColor( &gBlack );
	}
	
	static void PaintProgress( const Rect& insetBounds )
	{
		if ( const bool has_color_QD = MacFeatures::Has_ColorQuickdraw() )
		{
			PaintRect_In_Color( insetBounds, gDarkGrey );
		}
		else
		{
			PaintRect( &insetBounds );
		}
	}
	
	static void EraseProgress( const Rect& insetBounds )
	{
		if ( const bool has_color_QD = MacFeatures::Has_ColorQuickdraw() )
		{
			PaintRect_In_Color( insetBounds, gSkyBlue );
		}
		else
		{
			EraseRect( &insetBounds );
		}
	}
	
	void ProgressBar::DrawProgress( Rect insetBounds )
	{
		int value = Value();
		
		if ( value < 0 )
		{
			value = 0;
		}
		
		if ( value > 10000 )
		{
			value = 10000;
		}
		
		const short left = insetBounds.left;
		
		const int boundsWidth = insetBounds.right - insetBounds.left;
		
		int progressWidth = value * boundsWidth / 10000;
		
		insetBounds.left += short( progressWidth );
		
		EraseProgress( insetBounds );
		
		insetBounds.right = insetBounds.left;
		insetBounds.left  = left;
		
		PaintProgress( insetBounds );
	}
	
	void ProgressBar::Draw( const Rect& bounds, bool erasing )
	{
		FrameRect( &bounds );
		
		Rect insetBounds = bounds;
		
		InsetRect( &insetBounds, 1, 1 );
		
		DrawProgress( insetBounds );
	}
	
}
