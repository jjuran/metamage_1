/*	==============
 *	ProgressBar.hh
 *	==============
 */

#ifndef PEDESTAL_PROGRESSBAR_HH
#define PEDESTAL_PROGRESSBAR_HH

// Mac OS X
#ifdef __APPLE__
#include <ApplicationServices/ApplicationServices.h>
#endif

// Mac OS
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif

// Pedestal
#ifndef PEDESTAL_VIEW_HH
#include "Pedestal/View.hh"
#endif


namespace Pedestal
{
	
	class ProgressBar : public View
	{
		public:
			virtual double Progress() = 0;
			
			void DrawProgress( Rect insetBounds );
			
			void Draw( const Rect& bounds, bool erasing );
	};
	
}

#endif

