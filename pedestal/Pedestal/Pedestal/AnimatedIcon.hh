/*	===============
 *	AnimatedIcon.hh
 *	===============
 */

#ifndef PEDESTAL_ANIMATEDICON_HH
#define PEDESTAL_ANIMATEDICON_HH

// Nitrogen
#include "Nitrogen/QDOffScreen.h"

// Pedestal
#include "Pedestal/View.hh"


namespace Pedestal
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	
	class AnimatedIcon : public View
	{
		private:
			Rect bounds;
			NN::Owned< N::GWorldPtr, N::GWorldDisposer > myGWorld;
			bool mAnimating;
			long mCurrentFrame;
			long mTicksPerFrame;
			long mLastAdvance;
		
		public:
			AnimatedIcon( const Rect& bounds, Initializer init );
			
			const Rect& Bounds() const  { return bounds; }
			
			void Animate();
			void Draw();
			
			void Update();
			
			void MouseDown( const EventRecord& event );
			
			void Idle( const EventRecord& event )  { Animate(); }
			
			bool SetCursor( Point location, RgnHandle mouseRgn );
	};
	
}

#endif

