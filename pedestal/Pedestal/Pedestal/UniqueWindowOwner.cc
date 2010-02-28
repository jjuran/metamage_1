/*	====================
 *	UniqueWindowOwner.cc
 *	====================
 */

#include "Pedestal/UniqueWindowOwner.hh"

// Nitrogen
#include "Nitrogen/MacWindows.hh"


namespace Pedestal
{
	
	namespace N = Nitrogen;
	
	
	void UniqueWindowOwner::Show()
	{
		if ( Window* window = itsWindow.get() )
		{
			N::SelectWindow( window->Get() );
		}
		else
		{
			itsWindow = itsFactory();
			
			itsWindow->SetCloseHandler( itsCloseHandler );
		}
	}
	
}

