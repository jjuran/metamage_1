/*	==============
 *	ProgressBar.hh
 *	==============
 */

#pragma once

#include "Pedestal/View.hh"


namespace Pedestal
{
	
	class ProgressBar : public View
	{
		public:
			ProgressBar( const Rect& bounds );
			
			void ZeroProgress();
			void SetProgress( double progress );
			void DrawProgress();
			void Update();
		
		private:
			Rect myBounds;
			double myProgress;
	};
	
}

