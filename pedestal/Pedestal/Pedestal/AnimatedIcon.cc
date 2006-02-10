/*	===============
 *	AnimatedIcon.cc
 *	===============
 */

#include "Pedestal/AnimatedIcon.hh"

// Nitrogen
#include "Nitrogen/Icons.h"
#include "Nucleus/Scoped.h"

// Nitrogen Extras / Utilities
#include "Utilities/RectangleMath.h"


namespace Pedestal
{
	
	namespace NX = NitrogenExtras;
	
	enum
	{
		kFrameCount = 64  // must be positive
	};
	
	AnimatedIcon::AnimatedIcon( const Rect& bounds, Initializer )
	:
		bounds        ( bounds ),
	  	myGWorld      ( N::NewGWorld( 0, N::LocalToGlobal( bounds ) ) ),
	  	mAnimating    ( false ),
	  	mCurrentFrame ( 0 ),
	  	mTicksPerFrame( 2 ),
	  	mLastAdvance  ( 0 )
	{
	}
	
	void AnimatedIcon::Draw()
	{
		long range = 64;
		long eccentricity = 2 * ( mCurrentFrame - (kFrameCount / 2) ) * (range / kFrameCount);
		
		if ( eccentricity < 0 )
		{
			eccentricity = -eccentricity;
		}
		
		// Save GWorld
		N::GWorld_State savedGWorld = N::GetGWorld();
		
		// Lock pixels
		N::PixelsState_Details details( N::GetGWorldPixMap( myGWorld ) );
		NN::Scoped< N::PixelsState > savedPixelsState( N::PixelsState( details ) );
		
		N::LockPixels( N::GetGWorldPixMap( myGWorld ) );
		
		// Set source GWorld
		N::SetGWorld( myGWorld );
		
		Rect gworldBounds = N::GetPortBounds( myGWorld );
		
		Rect iconRect = N::InsetRect( bounds, eccentricity, range - eccentricity );
		
		// Draw source -- doesn't throw exceptions
		N::EraseRect( gworldBounds );
		N::PlotIconID( iconRect, kAlignAbsoluteCenter, kTransformNone, N::ResID( 128 ) );
		
		// Restore dest GWorld
		N::SetGWorld( savedGWorld );
		
		// Copy to dest
		N::CopyBits( N::GetPortBitMapForCopyBits( myGWorld                 ),
		             N::GetPortBitMapForCopyBits( N::GetQDGlobalsThePort() ),
		             N::GetPortBounds( myGWorld ),
		             bounds,
		             srcCopy );
		
		// Unlock pixels
	}
	
	void AnimatedIcon::Animate()
	{
		if ( mAnimating )
		{
			long ticks = ::TickCount();
			
			if ( ticks - mLastAdvance > mTicksPerFrame )
			{
				mCurrentFrame = (mCurrentFrame + 1) % kFrameCount;
				mLastAdvance = ticks;
				
				Draw();
			}
		}
	}
	
	void AnimatedIcon::Update()
	{
		//N::EraseRect( bounds );
		
		Draw();
	}
	
	void AnimatedIcon::MouseDown( const EventRecord& event )
	{
		mAnimating = !mAnimating;
	}
	
	bool AnimatedIcon::SetCursor( Point location, RgnHandle mouseRgn )
	{
		if ( N::PtInRect( location, Bounds() ) )
		{
			N::SetCursor( N::GetCursor( crossCursor ) );
			return true;
		}
		else
		{
			return false;
		}
	}
	
}

