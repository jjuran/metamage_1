/*	==============
 *	ProgressBar.cc
 *	==============
 */

#include "Pedestal/ProgressBar.hh"

// Nitrogen
#include "Nitrogen/Quickdraw.hh"


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
		PaintRect_In_Color( insetBounds, gDarkGrey );
	}
	
	static void EraseProgress( const Rect& insetBounds )
	{
		//N::EraseRect( insetBounds );
		
		PaintRect_In_Color( insetBounds, gSkyBlue );
	}
	
	ProgressBar::ProgressBar( const Rect& bounds )
	/*
	:
		itsBounds( CalcCenteredProgressBarRect( bounds ) ), 
		itsProgress( 0 )
	*/
	{
	}
	
	/*
	void ProgressBar::ZeroProgress()
	{
		//itsProgress = 0;
		
		// Erase the progress bar, leaving the black outline intact.
		EraseProgress( ProgressBarInsetBounds( itsBounds ) );
	}
	
	void ProgressBar::SetProgress( double progress )
	{
		itsProgress = progress;
		
		DrawProgress();
	}
	*/
	
	void ProgressBar::DrawProgress( Rect insetBounds )
	{
		double boundsWidth = insetBounds.right - insetBounds.left;
		
		double progressWidth = Progress() * boundsWidth;
		
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

