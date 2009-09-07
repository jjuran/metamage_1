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
#include <boost/shared_ptr.hpp>

// Pedestal
#include "Pedestal/Window.hh"


namespace Pedestal
{
	
	// A collection of windows of the same type.  Requests to close are always accepted.
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
					WindowStorage::iterator found = itsWindows.find( window );
					
					if ( found != itsWindows.end() )
					{
						itsWindows.erase( found );
					}
				}
		};
		
		private:
			WindowStorage                            itsWindows;
			boost::shared_ptr< WindowCloseHandler >  itsCloseHandler;
		
		public:
			WindowsOwner() : itsCloseHandler( new CloseHandler( itsWindows ) )
			{
			}
			
			void AddWindow( const boost::shared_ptr< Window >& window )
			{
				window->SetCloseHandler( itsCloseHandler );
				
				itsWindows[ window->Get() ] = window;
			}
	};
	
}

#endif

