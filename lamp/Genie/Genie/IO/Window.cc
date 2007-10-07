/*	=========
 *	Window.cc
 *	=========
 */

#include "Genie/IO/Window.hh"

// Standard C++
#include <algorithm>

// Lamp
#include "lamp/winio.h"

// Nitrogen
#include "Nitrogen/MacWindows.h"

// POSeven
#include "POSeven/Errno.hh"

// Pedestal
#include "Pedestal/Window.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	namespace p7 = poseven;
	namespace Ped = Pedestal;
	
	
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
				if ( argp != NULL )
				{
					N::Str255 title = GetTitle();
					
					std::copy( title + 1, title + 1 + title[0], (Byte*) argp );
					
					argp[ title[0] ] = '\0';
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
			
			default:
				IOHandle::IOCtl( request, argp );
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

