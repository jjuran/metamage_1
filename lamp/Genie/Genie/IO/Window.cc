/*	=========
 *	Window.cc
 *	=========
 */

#include "Genie/IO/Window.hh"

// Standard C
#include <signal.h>

// Nitrogen
#include "Nitrogen/MacWindows.h"

// Genie
#include "Genie/Process.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	namespace Ped = Pedestal;
	
	
	class DynamicWindowCloseHandler : public Ped::WindowCloseHandler
	{
		private:
			DynamicGroup&     itsGroup;
			DynamicElementID  itsID;
		
		public:
			DynamicWindowCloseHandler( DynamicGroup&     group,
			                           DynamicElementID  id ) : itsGroup( group ),
			                                                    itsID   ( id    )
			{
			}
			
			void operator()( N::WindowRef ) const;
	};
	
	class DynamicWindowResizeHandler : public Ped::WindowResizeHandler
	{
		private:
			DynamicGroup&     itsGroup;
			DynamicElementID  itsID;
		
		public:
			DynamicWindowResizeHandler( DynamicGroup&     group,
			                            DynamicElementID  id ) : itsGroup( group ),
			                                                     itsID   ( id    )
			{
			}
			
			void operator()( N::WindowRef window, short h, short v ) const;
	};
	
	
	void DynamicWindowCloseHandler::operator()( N::WindowRef ) const
	{
		const boost::shared_ptr< IOHandle >& handle( GetDynamicElementFromGroupByID( itsGroup, itsID ) );
		
		TerminalHandle& terminal( IOHandle_Cast< TerminalHandle >( *handle ) );
		
		terminal.Disconnect();
	}
	
	void DynamicWindowResizeHandler::operator()( N::WindowRef window, short h, short v ) const
	{
		N::SizeWindow( window, h, v, true );
		
		const boost::shared_ptr< IOHandle >& handle( GetDynamicElementFromGroupByID( itsGroup, itsID ) );
		
		TerminalHandle& terminal( IOHandle_Cast< TerminalHandle >( *handle ) );
		
		if ( !terminal.GetProcessGroup().expired() )
		{
			SendSignalToProcessGroup( SIGWINCH, *terminal.GetProcessGroup().lock() );
		}
	}
	
	boost::shared_ptr< Ped::WindowCloseHandler > GetDynamicWindowCloseHandler( DynamicGroup&     group,
	                                                                           DynamicElementID  id )
	{
		typedef boost::shared_ptr< Ped::WindowCloseHandler > Result;
		
		return Result( new DynamicWindowCloseHandler( group, id ) );
	}
	
	boost::shared_ptr< Ped::WindowResizeHandler > GetDynamicWindowResizeHandler( DynamicGroup&     group,
	                                                                             DynamicElementID  id )
	{
		typedef boost::shared_ptr< Ped::WindowResizeHandler > Result;
		
		return Result( new DynamicWindowResizeHandler( group, id ) );
	}
	
	
	WindowHandle::WindowHandle( const Ped::NewWindowContext&  context,
			                    Nitrogen::WindowDefProcID     procID,
			                    const std::string&            name ) : Ped::UserWindow( context, procID ),
			                                                           TerminalHandle( name )
	{
	}
	
	WindowHandle::~WindowHandle()
	{
	}
	
}

