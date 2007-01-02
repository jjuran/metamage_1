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
	
	static void PaintProgress( const Rect& bounds )
	{
		N::RGBForeColor( gDarkGrey );
		
		N::PaintRect( bounds );
		
		N::RGBForeColor( gBlack );
	}
	
	static void EraseProgress( const Rect& bounds )
	{
		//N::EraseRect( bounds );
		
		N::RGBForeColor( gSkyBlue );
		
		N::PaintRect( bounds );
		
		N::RGBForeColor( gBlack );
	}
	
	ProgressBar::ProgressBar( const Rect& bounds, View::Initializer init )
	:
		itsBounds( CalcCenteredProgressBarRect( bounds ) ), 
		itsProgress( 0 )
	{
		
	}
	
	void ProgressBar::ZeroProgress()
	{
		itsProgress = 0;
		
		// Erase the progress bar, leaving the black outline intact.
		EraseProgress( ProgressBarInsetBounds( itsBounds ) );
	}
	
	void ProgressBar::SetProgress( double progress )
	{
		itsProgress = progress;
		
		DrawProgress();
	}
	
	void ProgressBar::DrawProgress()
	{
		Rect bounds = ProgressBarInsetBounds( itsBounds );
		
		double boundsWidth = NX::RectWidth( bounds );
		double progressWidth = itsProgress * boundsWidth;
		
		bounds.right = bounds.left + short( progressWidth );
		
		PaintProgress( bounds );
	}
	
	void ProgressBar::Update()
	{
		N::FrameRect( itsBounds );
		
		EraseProgress( ProgressBarInsetBounds( itsBounds ) );
		
		DrawProgress();
	}
	
}

