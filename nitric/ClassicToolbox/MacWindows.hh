/*	=============
 *	MacWindows.hh
 *	=============
 */

#ifndef CLASSICTOOLBOX_MACWINDOWS_HH
#define CLASSICTOOLBOX_MACWINDOWS_HH

// Nostalgia
#include "Nostalgia/MacWindows.hh"

// Nitrogen
#ifndef NITROGEN_MACWINDOWS_HH
#include "Nitrogen/MacWindows.hh"
#endif


namespace Nitrogen
{
	
#if CALL_NOT_IN_CARBON
	
	struct WindowCloser
	{
		typedef WindowRef  argument_type;
		typedef void       result_type;
		
		void operator()( WindowRef window ) const
		{
			::CloseWindow( window );
		}
	};
	
	inline void MacCloseWindow( nucleus::owned< WindowRef, WindowCloser > )  {}
	
#endif
	
	inline void InvalRect( const Rect& r )  { ::InvalRect( &r ); }
	inline void ValidRect( const Rect& r )  { ::ValidRect( &r ); }
	
	using ::InvalRgn;
	using ::ValidRgn;
	
}

#endif

