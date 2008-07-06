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

// Nucleus
#include "Nucleus/Saved.h"

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
	
	boost::shared_ptr< Ped::WindowCloseHandler > GetDynamicWindowCloseHandler( DynamicGroup&     group,
			                                                                   DynamicElementID  id )
	{
		typedef boost::shared_ptr< Ped::WindowCloseHandler > Result;
		
		return Result( new DynamicWindowCloseHandler( group, id ) );
	}
	
	
	WindowHandle::WindowHandle( const std::string& name ) : TerminalHandle( name ),
	                                                        itsWindowSalvagePolicy( kLampSalvageWindowOnExitNever )
	{
	}
	
	WindowHandle::~WindowHandle()
	{
	}
	
	bool WindowHandle::ShouldBeSalvaged() const
	{
		if ( IsDisconnected() )
		{
			return false;
		}
		
		switch ( itsWindowSalvagePolicy )
		{
			default:
			case kLampSalvageWindowOnExitNever:
				//return false;
			
			case kLampSalvageWindowOnExitForSignal:
				// FIXME
			
			case kLampSalvageWindowOnExitForFailure:
				return GetLeaderWaitStatus() != 0;
			
			case kLampSalvageWindowOnExitAlways:
				return true;
		}
	}
	
	
	static Point GetWindowSize( N::WindowRef window )
	{
		Rect bounds = N::GetPortBounds( N::GetWindowPort( window ) );
		
		Point result;
		
		result.h = bounds.right - bounds.left;
		result.v = bounds.bottom - bounds.top;
		
		return result;
	}
	
	static void SetWindowSize( N::WindowRef window, Point size )
	{
		N::SizeWindow( window, size.h, size.v, true );
		
		if ( Ped::WindowBase* base = N::GetWRefCon( window ) )
		{
			base->Resized( N::GetPortBounds( N::GetWindowPort( window ) ) );
		}
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
					Point size = GetWindowSize( GetWindowRef() );
					
					*(Point*) argp = size;
				}
				
				break;
			
			case WIOCSSIZE:
				if ( argp == NULL )
				{
					p7::throw_errno( EFAULT );
				}
				
				SetWindowSize( GetWindowRef(), *(Point*) argp );
				
				break;
			
			/*
			case WIOCGDIM:
			case WIOCSDIM:
			*/
			
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
			
			case WIOCGEXIT:
				if ( argp != NULL )
				{
					*argp = itsWindowSalvagePolicy;
				}
				
				break;
			
			case WIOCSEXIT:
				if ( argp == NULL )
				{
					p7::throw_errno( EFAULT );
				}
				
				itsWindowSalvagePolicy = *argp;
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
		NN::Saved< N::Port_Value > savedPort;
		
		N::SetPortWindowPort( GetWindowRef() );
		
		Point upperLeft = { 0, 0 };
		
		::LocalToGlobal( &upperLeft );
		
		return upperLeft;
	}
	
	void WindowHandle::SetPosition( Point position )
	{
		N::MoveWindow( GetWindowRef(), position );
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

