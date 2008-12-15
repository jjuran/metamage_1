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
	
	
	void EmptyView::Update()
	{
		N::EraseRect( N::GetPortBounds( N::GetQDGlobalsThePort() ) );
	}
	
}

