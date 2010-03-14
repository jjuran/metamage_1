/*	=============
 *	GWorldView.cc
 *	=============
 */

#include "Pedestal/GWorldView.hh"

// Nucleus
#include "Nucleus/Saved.h"


namespace Pedestal
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	
	GWorldView::GWorldView( const Rect& bounds )
	:
		itsBounds( bounds ),
	  	itsGWorld( N::NewGWorld( 0, N::LocalToGlobal( bounds ) ) )
	{
		Erase( bounds );
	}
	
	void GWorldView::Erase( const Rect& bounds )
	{
		NN::Saved< N::GWorld > savedGWorld;
		
		N::SetGWorld( itsGWorld );
		
		N::EraseRect( bounds );
	}
	
	void GWorldView::Resize( short width, short height )
	{
		itsBounds.right = itsBounds.left + width;
		itsBounds.bottom = itsBounds.top + height;
		
		N::UpdateGWorld( itsGWorld, 0, itsBounds );
		
		Erase( itsBounds );
	}
	
	void GWorldView::Draw( const Rect& bounds, bool erasing )
	{
		// Lock pixels
		PixMapHandle pix = N::GetGWorldPixMap( itsGWorld );
		NN::Saved< N::PixelsState_Value > savedPixelsState( pix );
		
		N::LockPixels( pix );
		
		// Copy to dest
		N::CopyBits( N::GetPortBitMapForCopyBits( itsGWorld                 ),
		             N::GetPortBitMapForCopyBits( N::GetQDGlobalsThePort() ),
		             N::GetPortBounds( itsGWorld ),
		             bounds,
		             N::srcCopy );
		
		// Unlock pixels
	}
	
}

