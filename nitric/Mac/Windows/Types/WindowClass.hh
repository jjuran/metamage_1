/*
	Mac/Windows/Types/WindowClass.hh
	--------------------------------
*/

#ifndef MAC_WINDOWS_TYPES_WINDOWCLASS_HH
#define MAC_WINDOWS_TYPES_WINDOWCLASS_HH

// Mac OS X
#ifdef __APPLE__
#include <Carbon/Carbon.h>
#endif

// Mac OS
#ifndef __MACWINDOWS__
#include <MacWindows.h>
#endif

// nucleus
#ifndef NUCLEUS_ENUMERATIONTRAITS_HH
#include "nucleus/enumeration_traits.hh"
#endif


namespace Mac
{
	
	enum WindowClass
	{
		kAlertWindowClass        = ::kAlertWindowClass,
		kMovableAlertWindowClass = ::kMovableAlertWindowClass,
		kModalWindowClass        = ::kModalWindowClass,
		kMovableModalWindowClass = ::kMovableModalWindowClass,
		kFloatingWindowClass     = ::kFloatingWindowClass,
		kDocumentWindowClass     = ::kDocumentWindowClass,
		kUtilityWindowClass      = ::kUtilityWindowClass,
		kHelpWindowClass         = ::kHelpWindowClass,
		kSheetWindowClass        = ::kSheetWindowClass,
		kToolbarWindowClass      = ::kToolbarWindowClass,
		kPlainWindowClass        = ::kPlainWindowClass,
		kOverlayWindowClass      = ::kOverlayWindowClass,
		kSheetAlertWindowClass   = ::kSheetAlertWindowClass,
		kAltPlainWindowClass     = ::kAltPlainWindowClass,
		kDrawerWindowClass       = ::kDrawerWindowClass,
		kAllWindowClasses        = ::kAllWindowClasses,
		
		kWindowClass_Max = nucleus::enumeration_traits< ::WindowClass >::max
	};
	
}

#endif
