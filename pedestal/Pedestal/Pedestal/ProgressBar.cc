/*	==============
 *	ProgressBar.cc
 *	==============
 */

// Nitrogen Extras / Utilities
#include "Utilities/RectangleMath.h"

// Pedestal
#include "Pedestal/ProgressBar.hh"


namespace Pedestal
{
	
	namespace N = Nitrogen;
	namespace NX = NitrogenExtras;
	
	enum
	{
		kStdProgressBarHeight = 12, 
		kSideMargin = 12
	};
	
	static RGBColor gBlack = { 0, 0, 0 };
	
	static RGBColor gDarkGrey = { 0x4444, 0x4444, 0x4444 };
	static RGBColor gSkyBlue  = { 0xcccc, 0xcccc, 0xffff };
	
	
	static Rect CalcCenteredProgressBarRect( const Rect& frame )
	{
		return NX::CenteredRect( frame,
		                         N::SetPt( NX::RectWidth( frame ) - kSideMargin * 2,
		                                   kStdProgressBarHeight ) );
	}
	
	static Rect ProgressBarInsetBounds( const Rect& bounds )
	{
		return N::InsetRect( bounds, 1, 1 );
	}
	
	static void PaintProgress( const Rect& insetBounds )
	{
		N::RGBForeColor( gDarkGrey );
		
		N::PaintRect( insetBounds );
		
		N::RGBForeColor( gBlack );
	}
	
	static void EraseProgress( const Rect& insetBounds )
	{
		//N::EraseRect( insetBounds );
		
		N::RGBForeColor( gSkyBlue );
		
		N::PaintRect( insetBounds );
		
		N::RGBForeColor( gBlack );
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
		double boundsWidth = NX::RectWidth( insetBounds );
		
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

