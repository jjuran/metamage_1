/*	=======
 *	View.cc
 *	=======
 */

#include "Pedestal/View.hh"

// Nitrogen
#include "Nitrogen/QuickDraw.h"


namespace Pedestal
{
	
	namespace N = Nitrogen;
	
	
	void View::Draw( const Rect& bounds, bool erasing )
	{
		if ( erasing )
		{
			N::EraseRect( bounds );
		}
	}
	
	boost::shared_ptr< Quasimode > View::EnterShiftSpaceQuasimode( const EventRecord& )
	{
		return boost::shared_ptr< Quasimode >();
	}
	
}

