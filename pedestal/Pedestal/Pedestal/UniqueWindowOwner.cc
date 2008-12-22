/*	====================
 *	UniqueWindowOwner.cc
 *	====================
 */

#include "Pedestal/UniqueWindowOwner.hh"

// Nitrogen
#include "Nitrogen/MacWindows.h"


namespace Pedestal
{
	
	namespace N = Nitrogen;
	
	
	void UniqueWindowOwner::Show()
	{
		if ( Window* window = itsWindow.get() )
		{
			N::SelectWindow( window->Get() );
		}
		else if ( Window* created = itsFactory() )
		{
			itsWindow.reset( created );
			
			created->SetCloseHandler( itsCloseHandler );
		}
	}
	
}

