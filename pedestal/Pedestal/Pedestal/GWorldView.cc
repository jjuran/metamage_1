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
	
	GWorldView::GWorldView( const Rect& bounds, Initializer )
	:
		itsBounds( bounds ),
	  	itsGWorld( N::NewGWorld( 0, N::LocalToGlobal( bounds ) ) )
	{
	}
	
	void GWorldView::Resize( const Rect& bounds )
	{
		itsBounds = bounds;
		
		N::UpdateGWorld( itsGWorld, 0, bounds );
	}
	
	void GWorldView::Update()
	{
		// Lock pixels
		PixMapHandle pix = N::GetGWorldPixMap( itsGWorld );
		NN::Saved< N::PixelsState_Value > savedPixelsState( pix );
		
		N::LockPixels( pix );
		
		// Copy to dest
		N::CopyBits( N::GetPortBitMapForCopyBits( itsGWorld                 ),
		             N::GetPortBitMapForCopyBits( N::GetQDGlobalsThePort() ),
		             N::GetPortBounds( itsGWorld ),
		             itsBounds,
		             N::srcCopy );
		
		// Unlock pixels
	}
	
}

