/*	===============
 *	WindowsOwner.hh
 *	===============
 */

#ifndef PEDESTAL_WINDOWSOWNER_HH
#define PEDESTAL_WINDOWSOWNER_HH

// Standard C++
#include <memory>

// Boost
#include <boost/shared_ptr.hpp>

// Pedestal
#include "Pedestal/Window.hh"


namespace Pedestal
{
	
	// A collection of windows of the same type.  Requests to close are always accepted.
	template < class Window >
	class WindowsOwner
	{
		typedef std::map< ::WindowRef, boost::shared_ptr< Window > > WindowStorage;
		
		class CloseHandler : public WindowCloseHandler
		{
			private:
				WindowStorage& itsWindows;
			
			public:
				CloseHandler( WindowStorage& windows ) : itsWindows( windows  )
				{
				}
				
				void operator()( Nitrogen::WindowRef window ) const
				{
					typename WindowStorage::iterator found = itsWindows.find( window );
					
					if ( found != itsWindows.end() )
					{
						itsWindows.erase( found );
					}
				}
		};
		
		protected:
			WindowStorage                            itsWindows;
			boost::shared_ptr< WindowCloseHandler >  itsCloseHandler;
		
		public:
			WindowsOwner() : itsCloseHandler( new CloseHandler( itsWindows ) )
			{
			}
			
			void NewWindow()
			{
				Window* raw_ptr = new Window;
				
				boost::shared_ptr< Window > window( raw_ptr );
				
				raw_ptr->SetCloseHandler( itsCloseHandler );
				
				itsWindows[ raw_ptr->Get() ] = window;
			}
	};
	
}

#endif

