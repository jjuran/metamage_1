/*
	Mac/Windows/Types/WindowRef.hh
	-----------------------------
*/

#ifndef MAC_WINDOWS_TYPES_WINDOWREF_HH
#define MAC_WINDOWS_TYPES_WINDOWREF_HH

#ifndef __MACWINDOWS__
#include <MacWindows.h>
#endif

// nucleus
#ifndef NUCLEUS_DISPOSER_HH
#include "nucleus/disposer.hh"
#endif

// Nitrogen
#if !OPAQUE_TOOLBOX_STRUCTS
#ifndef NITROGEN_QUICKDRAW_HH
#include "Nitrogen/Quickdraw.hh"
#endif
#endif


namespace nucleus
{
	
#if OPAQUE_TOOLBOX_STRUCTS
	
	template <> struct disposer< WindowRef >
	{
		typedef WindowRef  argument_type;
		typedef void       result_type;
		
		void operator()( WindowRef w ) const
		{
			::DisposeWindow( w );
		}
	};
	
#endif
	
}

#endif

