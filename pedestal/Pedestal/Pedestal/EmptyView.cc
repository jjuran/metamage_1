/*	============
 *	EmptyView.cc
 *	============
 */

#include "Pedestal/EmptyView.hh"

// Nitrogen
#include "Nitrogen/MacWindows.h"


namespace Pedestal
{
	
	namespace N = Nitrogen;
	
	
	void EmptyView::Draw( const Rect& bounds )
	{
		N::EraseRect( bounds );
	}
	
}

