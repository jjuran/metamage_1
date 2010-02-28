/*	=============
 *	MacWindows.hh
 *	=============
 */

#ifndef CLASSICTOOLBOX_MACWINDOWS_HH
#define CLASSICTOOLBOX_MACWINDOWS_HH

// Nostalgia
#include "Nostalgia/MacWindows.hh"

// Nitrogen
#include "Nitrogen/MacWindows.h"


namespace Nitrogen
{
	
#if CALL_NOT_IN_CARBON
	
	struct WindowCloser : public std::unary_function< WindowRef, void >
	{
		void operator()( WindowRef window ) const
		{
			::CloseWindow( window );
		}
	};
	
	inline void MacCloseWindow( Nucleus::Owned< WindowRef, WindowCloser > )  {}
	
#endif
	
	inline void InvalRect( const Rect& r )  { ::InvalRect( &r ); }
	inline void ValidRect( const Rect& r )  { ::ValidRect( &r ); }
	
	using ::InvalRgn;
	using ::ValidRgn;
	
}

#endif

