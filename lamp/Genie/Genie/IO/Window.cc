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


namespace Genie
{
	
	namespace N = Nitrogen;
	namespace P7 = POSeven;
	
	
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
					P7::ThrowErrno( EFAULT );
				}
				
				SetPosition( *(Point*) argp );
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
		::MoveWindow( GetWindowRef(), position.h, position.v, false );
	}
	
}

