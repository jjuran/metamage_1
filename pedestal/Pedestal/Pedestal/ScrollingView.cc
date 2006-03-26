/*	================
 *	ScrollingView.cc
 *	================
 */

#include "Pedestal/ScrollingView.hh"


namespace Pedestal
{
	
	void ScopedOrigin::Setup( short h, short v, RgnHandle copyOfClipRegion )
	{
		// Set the new origin
		N::SetOrigin( h, v );
		
		// FIXME:  This part could be optimized for non-accessor use
		
		// Offset the clip region to account for the new origin
		::OffsetRgn( copyOfClipRegion, h, v );
		
		// Set it
		N::SetClip( copyOfClipRegion );
	}
	
}

