/*
	ProgressBar.cc
	--------------
*/

#include "Pedestal/ProgressBar.hh"

// Mac OS X
#ifdef __APPLE__
#include <ApplicationServices/ApplicationServices.h>
#endif

// Mac OS
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif

// missing-macos
#ifdef MAC_OS_X_VERSION_10_7
#ifndef MISSING_QUICKDRAW_H
#include "missing/Quickdraw.h"
#endif
#endif

// mac-config
#include "mac_config/color-quickdraw.hh"

// mac-sys-utils
#include "mac_sys/has/ColorQuickDraw.hh"


static inline
bool has_color_quickdraw()
{
	return CONFIG_COLOR_QUICKDRAW_GRANTED  ||  mac::sys::has_ColorQuickDraw();
}

namespace Pedestal
{
	
	enum
	{
		kStdProgressBarHeight = 12, 
		kSideMargin = 12
	};
	
	static const RGBColor gDarkGrey = { 0x4444, 0x4444, 0x4444 };
	static const RGBColor gSkyBlue  = { 0xcccc, 0xcccc, 0xffff };
	
	
	static void PaintRect_In_Color( const Rect& bounds, const RGBColor& color )
	{
		RGBForeColor( &color );
		
		PaintRect( &bounds );
		
		ForeColor( blackColor );
	}
	
	static void PaintProgress( const Rect& insetBounds )
	{
		if ( CONFIG_COLOR_QUICKDRAW  &&  has_color_quickdraw() )
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
		if ( CONFIG_COLOR_QUICKDRAW  &&  has_color_quickdraw() )
		{
			PaintRect_In_Color( insetBounds, gSkyBlue );
		}
		else
		{
			EraseRect( &insetBounds );
		}
	}
	
	static
	void DrawProgress( Rect insetBounds, int value )
	{
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
		
		DrawProgress( insetBounds, Value() );
	}
	
}
