/*	==============
 *	ProgressBar.hh
 *	==============
 */

#ifndef PEDESTAL_PROGRESSBAR_HH
#define PEDESTAL_PROGRESSBAR_HH

// Mac OS
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif

// Pedestal
#include "Pedestal/View.hh"


namespace Pedestal
{
	
	class ProgressBar : public View
	{
		private:
			Rect    itsBounds;
			//double  itsProgress;
		
		public:
			ProgressBar( const Rect& bounds );
			
			virtual double Progress() = 0;
			
			//void ZeroProgress();
			
			//void SetProgress( double progress );
			
			void DrawProgress( Rect insetBounds );
			
			void Draw( const Rect& bounds, bool erasing );
	};
	
}

#endif

