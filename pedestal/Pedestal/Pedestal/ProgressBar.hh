/*
	ProgressBar.hh
	--------------
*/

#ifndef PEDESTAL_PROGRESSBAR_HH
#define PEDESTAL_PROGRESSBAR_HH

// Pedestal
#ifndef PEDESTAL_VIEW_HH
#include "Pedestal/View.hh"
#endif


namespace Pedestal
{
	
	class ProgressBar : public View
	{
		public:
			virtual int Value() const = 0;
			
			void Draw( const Rect& bounds, bool erasing );
	};
	
}

#endif
