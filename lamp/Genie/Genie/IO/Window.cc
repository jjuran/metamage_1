/*	=========
 *	Window.cc
 *	=========
 */

#include "Genie/IO/Window.hh"

// Standard C++
#include <algorithm>

// Standard C
#include <signal.h>

// Lamp
#include "lamp/winio.h"

// Nitrogen
#include "Nitrogen/MacWindows.h"

// POSeven
#include "POSeven/Errno.hh"

// Pedestal
#include "Pedestal/Window.hh"

// Genie
#include "Genie/Process.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	namespace p7 = poseven;
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
		
		if ( !terminal.GetProcessGroup().expired() )
		{
			SendSignalToProcessGroup( SIGHUP, *terminal.GetProcessGroup().lock() );
		}
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
	
	
	WindowHandle::WindowHandle( const std::string& name ) : TerminalHandle( name )
	{
	}
	
	WindowHandle::~WindowHandle()
	{
	}
	
	
	void WindowHandle::IOCtl( unsigned long request, int* argp )
	{
		switch ( request )
		{
			case WIOCGTITLE:
				if ( Byte* buffer = (Byte*) argp )
				{
					N::Str255 title = GetTitle();
					
					std::copy( title + 1,
					           title + 1 + title[0],
					           buffer );
					
					buffer[ title[0] ] = '\0';
				}
				
				break;
			
			case WIOCSTITLE:
				SetTitle( argp ? N::Str255( (const char*) argp ) : NULL );
				
				break;
			
			case WIOCGPOS:
				if ( argp != NULL )
				{
					Point position = GetPosition();
					
					*(Point*) argp = position;
				}
				
				break;
			
			case WIOCSPOS:
				if ( argp == NULL )
				{
					p7::throw_errno( EFAULT );
				}
				
				SetPosition( *(Point*) argp );
				break;
			
			case WIOCGSIZE:
				if ( argp != NULL )
				{
					Point size = Ped::GetWindowSize( GetWindowRef() );
					
					*(Point*) argp = size;
				}
				
				break;
			
			case WIOCSSIZE:
				if ( argp == NULL )
				{
					p7::throw_errno( EFAULT );
				}
				
				Ped::SetWindowSize( GetWindowRef(), *(Point*) argp );
				
				break;
			
			case WIOCGVIS:
				if ( argp != NULL )
				{
					bool visible = IsVisible();
					
					*argp = visible;
				}
				
				break;
			
			case WIOCSVIS:
				if ( argp == NULL )
				{
					p7::throw_errno( EFAULT );
				}
				
				if ( *argp )
				{
					Show();
				}
				else
				{
					Hide();
				}
				
				break;
			
			default:
				TerminalHandle::IOCtl( request, argp );
				break;
		}
	}
	
	Nitrogen::Str255 WindowHandle::GetTitle() const
	{
		return N::GetWTitle( GetWindowRef() );
	}
	
	void WindowHandle::SetTitle( ConstStr255Param title )
	{
		N::SetWTitle( GetWindowRef(), title );
	}
	
	Point WindowHandle::GetPosition() const
	{
		return Ped::GetWindowPosition( GetWindowRef() );
	}
	
	void WindowHandle::SetPosition( Point position )
	{
		Ped::SetWindowPosition( GetWindowRef(), position );
	}
	
	bool WindowHandle::IsVisible() const
	{
		return N::IsWindowVisible( GetWindowRef() );
	}
	
	void WindowHandle::Show() const
	{
		N::ShowWindow( GetWindowRef() );
	}
	
	void WindowHandle::Hide() const
	{
		N::HideWindow( GetWindowRef() );
	}
	
}

