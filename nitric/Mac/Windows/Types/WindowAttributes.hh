/*
	Mac/Windows/Types/WindowAttributes.hh
	--------------------------------
*/

#ifndef MAC_WINDOWS_TYPES_WINDOWATTRIBUTES_HH
#define MAC_WINDOWS_TYPES_WINDOWATTRIBUTES_HH

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
#ifndef NUCLEUS_FLAGOPS_HH
#include "nucleus/flag_ops.hh"
#endif


namespace Mac
{
	
	enum WindowAttributes
	{
		kWindowNoAttributes               = ::kWindowNoAttributes,
		kWindowCloseBoxAttribute          = ::kWindowCloseBoxAttribute,
		kWindowHorizontalZoomAttribute    = ::kWindowHorizontalZoomAttribute,
		kWindowVerticalZoomAttribute      = ::kWindowVerticalZoomAttribute,
		kWindowFullZoomAttribute          = ::kWindowFullZoomAttribute,
		kWindowCollapseBoxAttribute       = ::kWindowCollapseBoxAttribute,
		kWindowResizableAttribute         = ::kWindowResizableAttribute,
		kWindowSideTitlebarAttribute      = ::kWindowSideTitlebarAttribute,
		kWindowToolbarButtonAttribute     = ::kWindowToolbarButtonAttribute,
		kWindowNoUpdatesAttribute         = ::kWindowNoUpdatesAttribute,
		kWindowNoActivatesAttribute       = ::kWindowNoActivatesAttribute,
		kWindowOpaqueForEventsAttribute   = ::kWindowOpaqueForEventsAttribute,
		
	#ifdef MAC_OS_X_VERSION_10_2
		
		kWindowCompositingAttribute       = ::kWindowCompositingAttribute,
		
	#endif
		
	#ifdef MAC_OS_X_VERSION_10_3
		
		kWindowAsyncDragAttribute         = ::kWindowAsyncDragAttribute,
		
	#endif
		
	#ifdef MAC_OS_X_VERSION_10_7
		
		kWindowHighResolutionCapableAttribute =
			::kWindowHighResolutionCapableAttribute,
		
	#endif
		
		kWindowNoShadowAttribute          = ::kWindowNoShadowAttribute,
		kWindowHideOnSuspendAttribute     = ::kWindowHideOnSuspendAttribute,
		kWindowStandardHandlerAttribute   = ::kWindowStandardHandlerAttribute,
		kWindowHideOnFullScreenAttribute  = ::kWindowHideOnFullScreenAttribute,
		kWindowInWindowMenuAttribute      = ::kWindowInWindowMenuAttribute,
		kWindowLiveResizeAttribute        = ::kWindowLiveResizeAttribute,
		kWindowNoConstrainAttribute       = ::kWindowNoConstrainAttribute,
		kWindowStandardDocumentAttributes = ::kWindowStandardDocumentAttributes,
		kWindowStandardFloatingAttributes = ::kWindowStandardFloatingAttributes,
		
		kWindowAttributes_Max = nucleus::enumeration_traits< ::WindowAttributes >::max
	};
	
	NUCLEUS_DEFINE_FLAG_OPS( WindowAttributes )
	
}

#endif
