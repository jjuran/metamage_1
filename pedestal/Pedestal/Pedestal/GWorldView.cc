/*	=============
 *	GWorldView.cc
 *	=============
 */

#include "Pedestal/GWorldView.hh"

// Nitrogen
#include "Nitrogen/Scoped.h"


namespace Pedestal
{
	
	GWorldView::GWorldView( const Rect& bounds )
	:
		bounds        ( bounds ),
	  	myGWorld      ( N::NewGWorld( 0, N::LocalToGlobal( bounds ) ) )
	{
	}
	
	void GWorldView::Update()
	{
		// Lock pixels
		N::PixelsState_Details details( N::GetGWorldPixMap( myGWorld ) );
		N::Scoped< N::PixelsState > savedPixelsState( N::PixelsState( details ) );
		
		N::LockPixels( N::GetGWorldPixMap( myGWorld ) );
		
		// Copy to dest
		N::CopyBits( N::GetPortBitMapForCopyBits( myGWorld                 ),
		             N::GetPortBitMapForCopyBits( N::GetQDGlobalsThePort() ),
		             N::GetPortBounds( myGWorld ),
		             bounds,
		             srcCopy );
		
		// Unlock pixels
	}
	
}

