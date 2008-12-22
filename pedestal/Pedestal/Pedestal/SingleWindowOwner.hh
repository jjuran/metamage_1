/*	====================
 *	SingleWindowOwner.hh
 *	====================
 */

#ifndef PEDESTAL_SINGLEWINDOWOWNER_HH
#define PEDESTAL_SINGLEWINDOWOWNER_HH

// Standard C++
#include <memory>

// Pedestal
#include "Pedestal/Window.hh"


namespace Pedestal
{
	
	// A single window that exists for the duration of the owner.
	// This could be a member of a document object.
	template < class Window >
	class SingleWindowOwner
	{
		typedef std::auto_ptr< Window > WindowStorage;
		
		protected:
			WindowStorage itsWindow;
		
		public:
			SingleWindowOwner( const boost::shared_ptr< WindowCloseHandler >& handler ) : itsWindow( new Window( handler ) )
			{
			}
			
			Window const& Get() const  { return *itsWindow.get(); }
			Window      & Get()        { return *itsWindow.get(); }
	};
	
}

#endif

