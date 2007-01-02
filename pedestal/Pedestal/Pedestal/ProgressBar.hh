/*	==============
 *	ProgressBar.hh
 *	==============
 */

#ifndef PEDESTAL_PROGRESSBAR_HH
#define PEDESTAL_PROGRESSBAR_HH

#include "Pedestal/View.hh"


namespace Pedestal
{
	
	class ProgressBar : public View
	{
		private:
			Rect    itsBounds;
			double  itsProgress;
		
		public:
			ProgressBar( const Rect& bounds, View::Initializer init = View::Initializer() );
			
			void ZeroProgress();
			void SetProgress( double progress );
			void DrawProgress();
			void Update();
	};
	
}

#endif

