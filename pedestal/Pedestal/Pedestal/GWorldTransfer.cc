/*	==========================
 *	Pedestal/GWorldTransfer.cc
 *	==========================
 */

#include "Pedestal/GWorldTransfer.hh"

// Nitrogen
#include "Nitrogen/Icons.h"


namespace Pedestal
{
	
	namespace N = Nitrogen;
	
	void GWorldTransfer::Plot( const Rect& area ) const
	{
		N::CopyBits( N::GetPortBitMapForCopyBits( fGWorld                  ),
		             N::GetPortBitMapForCopyBits( N::GetQDGlobalsThePort() ),
		             N::GetPortBounds( fGWorld ),
		             area,
		             fMode );
	}
	
}

