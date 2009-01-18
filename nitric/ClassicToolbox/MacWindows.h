/*	============
 *	MacWindows.h
 *	============
 */

#ifndef CLASSICTOOLBOX_MACWINDOWS_H
#define CLASSICTOOLBOX_MACWINDOWS_H

// Nostalgia
#include "Nostalgia/MacWindows.hh"

// Nitrogen
#include "Nitrogen/MacWindows.h"


namespace Nitrogen
{
	
	struct WindowCloser : public std::unary_function< WindowRef, void >
	{
		void operator()( WindowRef window ) const
		{
			::CloseWindow( window );
		}
	};
	
	inline void MacCloseWindow( Nucleus::Owned< WindowRef, WindowCloser > )  {}
	
	inline void InvalRect( const Rect& r )  { ::InvalRect( &r ); }
	inline void ValidRect( const Rect& r )  { ::ValidRect( &r ); }
	
	using ::InvalRgn;
	using ::ValidRgn;
	
}

#endif

