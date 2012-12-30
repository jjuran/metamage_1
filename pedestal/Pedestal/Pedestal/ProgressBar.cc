/*	==============
 *	ProgressBar.cc
 *	==============
 */

#include "Pedestal/ProgressBar.hh"

// Nitrogen
#include "Nitrogen/Quickdraw.hh"

// MacFeatures
#include "MacFeatures/ColorQuickdraw.hh"


namespace Pedestal
{
	
	namespace N = Nitrogen;
	
	enum
	{
		kStdProgressBarHeight = 12, 
		kSideMargin = 12
	};
	
	static RGBColor gBlack = { 0, 0, 0 };
	
	static RGBColor gDarkGrey = { 0x4444, 0x4444, 0x4444 };
	static RGBColor gSkyBlue  = { 0xcccc, 0xcccc, 0xffff };
	
	
	static Rect ProgressBarInsetBounds( const Rect& bounds )
	{
		return N::InsetRect( bounds, 1, 1 );
	}
	
	static void PaintRect_In_Color( const Rect& bounds, const RGBColor& color )
	{
		N::RGBForeColor( color );
		
		N::PaintRect( bounds );
		
		N::RGBForeColor( gBlack );
	}
	
	static void PaintProgress( const Rect& insetBounds )
	{
		if ( const bool has_color_QD = MacFeatures::Has_ColorQuickdraw() )
		{
			PaintRect_In_Color( insetBounds, gDarkGrey );
		}
		else
		{
			N::PaintRect( insetBounds );
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
			N::EraseRect( insetBounds );
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
		
		const int boundsWidth = insetBounds.right - insetBounds.left;
		
		int progressWidth = value * boundsWidth / 10000;
		
		insetBounds.right = insetBounds.left + short( progressWidth );
		
		PaintProgress( insetBounds );
	}
	
	void ProgressBar::Draw( const Rect& bounds, bool erasing )
	{
		N::FrameRect( bounds );
		
		Rect insetBounds = ProgressBarInsetBounds( bounds );
		
		EraseProgress( insetBounds );
		
		DrawProgress( insetBounds );
	}
	
}

