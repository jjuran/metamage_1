/*	=========
 *	Window.cc
 *	=========
 */

#include "Genie/IO/Window.hh"

// Standard C++
#include <algorithm>

// Lamp
#include "lamp/winio.h"


namespace Genie
{
	
	namespace N = Nitrogen;
	
	
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
	
}

