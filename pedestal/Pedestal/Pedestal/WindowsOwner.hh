/*	===============
 *	WindowsOwner.hh
 *	===============
 */

#ifndef PEDESTAL_WINDOWSOWNER_HH
#define PEDESTAL_WINDOWSOWNER_HH

// Standard C++
#include <map>

// Debug
#include "debug/boost_assert.hh"

// Boost
#include <boost/intrusive_ptr.hpp>

// Pedestal
#ifndef PEDESTAL_WINDOW_HH
#include "Pedestal/Window.hh"
#endif


namespace Pedestal
{
	
	// A collection of windows of the same type.  Requests to close are always accepted.
	class WindowsOwner
	{
		typedef std::map< ::WindowRef, boost::intrusive_ptr< Window > > WindowStorage;
		
		class CloseHandler : public WindowCloseHandler
		{
			private:
				WindowStorage& itsWindows;
			
			public:
				CloseHandler( WindowStorage& windows ) : itsWindows( windows  )
				{
				}
				
				void operator()( WindowRef window ) const
				{
					WindowStorage::iterator found = itsWindows.find( window );
					
					if ( found != itsWindows.end() )
					{
						itsWindows.erase( found );
					}
				}
		};
		
		private:
			WindowStorage                               itsWindows;
			boost::intrusive_ptr< WindowCloseHandler >  itsCloseHandler;
		
		public:
			WindowsOwner() : itsCloseHandler( new CloseHandler( itsWindows ) )
			{
			}
			
			void AddWindow( const boost::intrusive_ptr< Window >& window )
			{
				window->SetCloseHandler( itsCloseHandler );
				
				itsWindows[ window->Get() ] = window;
			}
	};
	
}

#endif

