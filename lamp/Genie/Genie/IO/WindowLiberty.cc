/*	================
 *	WindowLiberty.cc
 *	================
 */

#include "Genie/IO/WindowLiberty.hh"

// Standard C++
#include <map>

// Genie
#include "Genie/IO/ConsoleWindow.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	namespace Ped = Pedestal;
	
	
	std::map< ::WindowRef, boost::shared_ptr< IOHandle > > gFreeWindows;
	
	
	class FreeWindowCloseHandler : public Ped::WindowCloseHandler
	{
		public:
			void operator()( N::WindowRef window ) const;
	};
	
	void FreeWindowCloseHandler::operator()( N::WindowRef window ) const
	{
		gFreeWindows.erase( window );
	}
	
	void LiberateWindow( Pedestal::WindowCore&                 window,
	                     const boost::shared_ptr< IOHandle >&  handle )
	{
		ASSERT( handle.get() != NULL );
		
		boost::shared_ptr< Ped::WindowCloseHandler > handler( new FreeWindowCloseHandler() );
		
		window.SetCloseHandler( handler );
		
		gFreeWindows[ window.Get() ] = handle;
	}
	
}

