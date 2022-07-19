/*
	window.cc
	---------
*/

#include "aglow/window.hh"

// Mac OS X
#ifdef __APPLE__
#include <Carbon/Carbon.h>
#endif

// mac-config
#include "mac_config/upp-macros.hh"

// mac-qd-utils
#include "mac_qd/get_portRect.hh"
#include "mac_qd/main_display_bounds.hh"

// mac-ui-utils
#include "mac_ui/windows.hh"

// rasterlib
#include "raster/clut.hh"
#include "raster/clut_detail.hh"
#include "raster/load.hh"


using raster::raster_desc;


short x_numer = 1;
short x_denom = 1;

static inline
short rect_width( const Rect& r )
{
	return r.right - r.left;
}

static inline
short rect_height( const Rect& r )
{
	return r.bottom - r.top;
}

static
Rect get_desktop_bounds()
{
	Rect bounds = mac::qd::main_display_bounds();
	
	bounds.top    += 40;  // approximate menu bar height + title bar height
	bounds.bottom -=  2;  // approximate window frame thickness
	
	return bounds;
}

static Rect desktop_bounds = get_desktop_bounds();

static Point proportional_location =
{
	rect_height( desktop_bounds ) / 2u,
	rect_width ( desktop_bounds ) / 2u,
};

Fixed maximum_zoom( short window_width, short window_height )
{
	short desk_height = rect_height( desktop_bounds );
	short desk_width  = rect_width ( desktop_bounds );
	
	Fixed X_zoom = FixRatio( desk_width,  window_width  );
	Fixed Y_zoom = FixRatio( desk_height, window_height );
	
	Fixed max = X_zoom > Y_zoom ? X_zoom : Y_zoom;
	
	return max;
}

static
short get_proportional_coordinate( short outer, short inner, short offset )
{
	short delta = outer - inner;
	
	if ( delta <= 0 )
	{
		return 0;
	}
	
	return offset * outer / delta;
}

static
short get_proportional_offset( short outer, short inner, short coordinate )
{
	short delta = outer - inner;
	
	if ( delta <= 0 )
	{
		return 0;
	}
	
	return coordinate * delta / outer;
}

static
Point get_proportional_location( const Rect& bounds )
{
	short desk_height = rect_height( desktop_bounds );
	short desk_width  = rect_width ( desktop_bounds );
	
	short window_height = bounds.bottom - bounds.top;
	short window_width  = bounds.right - bounds.left;
	
	Point location =
	{
		get_proportional_coordinate( desk_height, window_height, bounds.top  ),
		get_proportional_coordinate( desk_width,  window_width,  bounds.left ),
	};
	
	return location;
}

static
Rect get_proportional_bounds( short width, short height, Point location )
{
	short desk_height = rect_height( desktop_bounds );
	short desk_width  = rect_width ( desktop_bounds );
	
	Rect bounds;
	
	bounds.top  = get_proportional_offset( desk_height, height, location.v );
	bounds.left = get_proportional_offset( desk_width,  width,  location.h );
	
	bounds.bottom = bounds.top + height;
	bounds.right = bounds.left + width;
	
	return bounds;
}

static
Rect get_window_bounds( const raster_desc& desc, UInt16 numer, UInt16 denom )
{
	short desk_height = rect_height( desktop_bounds );
	short desk_width  = rect_width ( desktop_bounds );
	
	unsigned window_height = desc.height;
	unsigned window_width  = desc.width;
	
	window_height /= denom;
	window_width  /= denom;
	
	window_height *= numer;
	window_width  *= numer;
	
	short v_margin = (desk_height - window_height) / 2u;
	short h_margin = (desk_width  - window_width ) / 2u;
	
	Rect bounds = desktop_bounds;
	
	bounds.top += v_margin;
	bounds.left += h_margin;
	bounds.right -= h_margin;
	bounds.bottom -= v_margin;
	
	return bounds;
}

static
void SetWindowTitleToProcessName( WindowRef window )
{
	ProcessSerialNumber psn = { 0, kCurrentProcess };
	
	OSStatus err;
	CFStringRef name;
	
	err = CopyProcessName( &psn, &name );
	
	if ( err == noErr )
	{
		SetWindowTitleWithCFString( window, name );
		
		CFRelease( name );
	}
}

WindowRef create_window( const raster_desc& desc, WindowRef previous )
{
	const WindowAttributes attrs = kWindowCloseBoxAttribute
	                             | kWindowCollapseBoxAttribute
	                           #ifdef MAC_OS_X_VERSION_10_3
	                             | kWindowCompositingAttribute
	                           #endif
	                           #ifdef MAC_OS_X_VERSION_10_3
	                             | kWindowAsyncDragAttribute
	                           #endif
	                           #ifdef MAC_OS_X_VERSION_10_7
	                             | kWindowHighResolutionCapableAttribute
	                           #endif
	                             ;
	
	unsigned window_height = desc.height;
	unsigned window_width  = desc.width;
	
	window_height /= x_denom;
	window_width  /= x_denom;
	
	window_height *= x_numer;
	window_width  *= x_numer;
	
	Rect bounds;
	
	if ( previous )
	{
		GetWindowBounds( previous, kWindowGlobalPortRgn, &bounds );
		
		OffsetRect( &bounds, -desktop_bounds.left, -desktop_bounds.top );
		
		proportional_location = get_proportional_location( bounds );
	}
	
	bounds = get_proportional_bounds( window_width,
	                                  window_height,
	                                  proportional_location );
	
	OffsetRect( &bounds, desktop_bounds.left, desktop_bounds.top );
	
	OSStatus err;
	WindowRef window;
	err = CreateNewWindow( kDocumentWindowClass, attrs, &bounds, &window );
	
	if ( err != noErr )
	{
		return NULL;
	}
	
	SetWindowTitleToProcessName( window );
	
	return window;
}
