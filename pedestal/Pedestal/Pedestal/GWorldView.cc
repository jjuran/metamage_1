/*	=============
 *	GWorldView.cc
 *	=============
 */

#include "Pedestal/GWorldView.hh"

// Nucleus
#include "Nucleus/Saved.h"


namespace Pedestal
{
	
	namespace NN = Nucleus;
	
	GWorldView::GWorldView( const Rect& bounds )
	:
		bounds        ( bounds ),
	  	myGWorld      ( N::NewGWorld( 0, N::LocalToGlobal( bounds ) ) )
	{
	}
	
	void GWorldView::Update()
	{
		// Lock pixels
		PixMapHandle pix = N::GetGWorldPixMap( myGWorld );
		NN::Saved< N::PixelsState_Value > savedPixelsState( pix );
		
		N::LockPixels( pix );
		
		// Copy to dest
		N::CopyBits( N::GetPortBitMapForCopyBits( myGWorld                 ),
		             N::GetPortBitMapForCopyBits( N::GetQDGlobalsThePort() ),
		             N::GetPortBounds( myGWorld ),
		             bounds,
		             srcCopy );
		
		// Unlock pixels
	}
	
}

