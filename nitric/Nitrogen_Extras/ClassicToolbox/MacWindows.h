/*	============
 *	MacWindows.h
 *	============
 */

#ifndef CLASSICTOOLBOX_MACWINDOWS_H
#define CLASSICTOOLBOX_MACWINDOWS_H

// Nitrogen core
#ifndef NITROGEN_OWNED_H
#include "Nitrogen/Owned.h"
#endif

// Nitrogen Carbon support
#ifndef NITROGEN_MACWINDOWS_H
#include "Nitrogen/MacWindows.h"
#endif


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
	
	inline void MacCloseWindow( Owned< WindowRef, WindowCloser > )  {}
	
	inline void InvalRect( const Rect& r )  { ::InvalRect( &r ); }
	
	using ::InvalRgn;
	
#else
	
	inline void InvalRect( const Rect& r )
	{
		::InvalWindowRect( ::GetWindowFromPort( ::GetQDGlobalsThePort() ), &r );
	}
	
	inline void InvalRgn( RgnHandle rgn )
	{
		::InvalWindowRgn( ::GetWindowFromPort( ::GetQDGlobalsThePort() ), rgn );
	}
	
#endif  // CALL_NOT_IN_CARBON
	
}

#endif

