/*	=============
 *	GWorldView.cc
 *	=============
 */

#include "Pedestal/GWorldView.hh"

// nucleus
#include "nucleus/saved.hh"


namespace Pedestal
{
	
	namespace n = nucleus;
	namespace N = Nitrogen;
	
	GWorldView::GWorldView( const Rect& bounds )
	:
		itsBounds( bounds ),
	  	itsGWorld( N::NewGWorld( 0, N::LocalToGlobal( bounds ) ) )
	{
		Erase( bounds );
	}
	
	void GWorldView::Erase( const Rect& bounds )
	{
		n::saved< N::GWorld > savedGWorld;
		
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
		n::saved< N::Pixels_State > savedPixelsState( pix );
		
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

