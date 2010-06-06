/*
	Mac/Windows/Types/WindowDefProcID.hh
	------------------------------------
*/

#ifndef MAC_WINDOWS_TYPES_WINDOWDEFPROCID_HH
#define MAC_WINDOWS_TYPES_WINDOWDEFPROCID_HH

#ifndef __MACWINDOWS__
#include <MacWindows.h>
#endif

// nucleus
#include "nucleus/enumeration_traits.hh"


namespace Mac
{
	
	enum WindowDefProcID
	{
		documentProc    = ::documentProc,
		dBoxProc        = ::dBoxProc,
		plainDBox       = ::plainDBox,
		altDBoxProc     = ::altDBoxProc,
		noGrowDocProc   = ::noGrowDocProc,
		movableDBoxProc = ::movableDBoxProc,
		zoomDocProc     = ::zoomDocProc,
		zoomNoGrow      = ::zoomNoGrow,
		
		kWindowDefProcID_Max = nucleus::enumeration_traits< ::SInt16 >::max
	};
	
}

#endif

