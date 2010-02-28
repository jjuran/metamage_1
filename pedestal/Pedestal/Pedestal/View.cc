/*	=======
 *	View.cc
 *	=======
 */

#include "Pedestal/View.hh"

// Nitrogen
#include "Nitrogen/Quickdraw.hh"

// Pedestal
#include "Pedestal/CurrentFocus.hh"


namespace Pedestal
{
	
	namespace N = Nitrogen;
	
	
	void View::Uninstall()
	{
		// This is only needed for focusable fields, but it's critical to ensure
		// that we don't continue to reference them after they've been destroyed.
		// Rather than count on remembering to override Uninstall() for every
		// field class, just do it here.
		
		Unfocus( this );
	}
	
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

