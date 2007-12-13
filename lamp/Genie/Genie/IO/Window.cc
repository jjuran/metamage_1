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
	namespace p7 = poseven;
	namespace Ped = Pedestal;
	
	
	static WindowMap gWindowMap;
	
	const WindowMap& GetWindowMap()
	{
		return gWindowMap;
	}
	
	void AddWindowToMap( ::WindowRef window, const boost::shared_ptr< IOHandle >& handle )
	{
		gWindowMap[ window ] = handle;
	}
	
	TerminalHandle& GetWindowFromMap( ::WindowRef window )
	{
		WindowMap::const_iterator it = gWindowMap.find( window );
		
		if ( it == gWindowMap.end() )
		{
			p7::throw_errno( ENOENT );
		}
		
		ASSERT( !it->second.expired() );
		
		return IOHandle_Cast< TerminalHandle >( *it->second.lock() );
	}
	
	
	class TerminalCloseHandler : public Ped::WindowCloseHandler
	{
		public:
			void operator()( N::WindowRef window ) const;
	};
	
	void TerminalCloseHandler::operator()( N::WindowRef window ) const
	{
		TerminalHandle& terminal = GetWindowFromMap( window );
		
		terminal.Disconnect();
		
		if ( !terminal.GetProcessGroup().expired() )
		{
			const boost::shared_ptr< ProcessGroup >& processGroup = terminal.GetProcessGroup().lock();
			
			SendSignalToProcessGroup( SIGHUP, *processGroup );
		}
	}
	
	const boost::shared_ptr< Ped::WindowCloseHandler >& GetTerminalCloseHandler()
	{
		typedef boost::shared_ptr< Ped::WindowCloseHandler > SharedHandler;
		
		static SharedHandler handler = SharedHandler( new TerminalCloseHandler() );
		
		return handler;
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
		PixMapHandle pixmap = ::GetPortPixMap( ::CGrafPtr( N::GetWindowPort( GetWindowRef() ) ) );
		
		Rect bounds = pixmap[0]->bounds;
		
		Point result = *(Point*) &bounds;
		
		result.h = -result.h;
		result.v = -result.v;
		
		return result;
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

