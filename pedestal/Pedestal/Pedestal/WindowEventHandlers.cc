/*
	WindowEventHandlers.cc
	----------------------
*/

#include "Pedestal/WindowEventHandlers.hh"

// Mac OS
#ifndef __CARBONEVENTS__
#include <CarbonEvents.h>
#endif

// mac-config
#include "mac_config/upp-macros.hh"

// mac-qd-utils
#include "mac_qd/get_portRect.hh"

// nucleus
#include "nucleus/saved.hh"

// Nitrogen
#include "Nitrogen/Quickdraw.hh"

// ClassicToolbox
#include "ClassicToolbox/MacWindows.hh"

// Pedestal
#include "Pedestal/View.hh"
#include "Pedestal/WindowStorage.hh"


using mac::qd::get_portRect;


namespace Pedestal
{
	
	namespace n = nucleus;
	namespace N = Nitrogen;
	
	
	static inline
	bool window_is_resizable( WindowRef window )
	{
		return get_window_attributes( window ) & kWindowResizableAttribute;
	}
	
	static inline
	bool window_has_grow_icon( WindowRef window )
	{
		/*
			Returns false if the window is not user-resizable, or if the
			grow box is definitely managed as part of the window frame.
			
			Returns true for resizable windows pre-Carbon, although in some
			cases (e.g. Mac OS 8 and 9) the grow box is part of the window
			frame and doesn't need to be redrawn/invalidated.  However, it's
			harmless to do so.
		*/
		
		return ! TARGET_API_MAC_CARBON  &&  window_is_resizable( window );
	}
	
	static
	Rect GrowBoxBounds( WindowRef window )
	{
		Rect bounds = get_portRect( GetWindowPort( window ) );
		
		bounds.left = bounds.right - 15;
		bounds.top = bounds.bottom - 15;
		
		return bounds;
	}
	
	
	void window_activated( WindowRef window, bool activating )
	{
		if ( View* view = get_window_view( window ) )
		{
			view->Activate( activating );
		}
		
		if ( window_has_grow_icon( window ) )
		{
			N::InvalRect( GrowBoxBounds( window ) );
		}
	}
	
	void window_mouseDown( WindowRef window, const EventRecord& event )
	{
		// FIXME:  The window may want clicks even if it's not in front.
		if ( window != FrontWindow() )
		{
			SelectWindow( window );
		}
		else if ( View* view = get_window_view( window ) )
		{
			view->MouseDown( event );
		}
	}
	
	void window_update( WindowRef window )
	{
		if ( View* view = get_window_view( window ) )
		{
			view->Draw( get_portRect( GetWindowPort( window ) ), true );
		}
		
		if ( window_has_grow_icon( window ) )
		{
			n::saved< N::Clip > savedClip;
			
			N::ClipRect( GrowBoxBounds( window ) );
			
			DrawGrowIcon( window );
		}
	}
	
	
	static
	pascal OSStatus window_WindowClose( EventHandlerCallRef  handler,
	                                    EventRef             event,
	                                    void*                userData )
	{
		OSStatus err;
		
		WindowRef window;
		err = GetEventParameter( event,
		                         kEventParamDirectObject,
		                         typeWindowRef,
		                         NULL,
		                         sizeof (WindowRef),
		                         NULL,
		                         &window );
		
		if ( err == noErr )
		{
			close_window( window );
		}
		
		return err;
	}
	
	DEFINE_CARBON_UPP( EventHandler, window_WindowClose )
	
	static EventTypeSpec windowClose_event[] =
	{
		{ kEventClassWindow, kEventWindowClose },
	};
	
	
	static
	pascal OSStatus window_WindowClosed( EventHandlerCallRef  handler,
	                                     EventRef             event,
	                                     void*                userData )
	{
		OSStatus err;
		
		WindowRef window;
		err = GetEventParameter( event,
		                         kEventParamDirectObject,
		                         typeWindowRef,
		                         NULL,
		                         sizeof (WindowRef),
		                         NULL,
		                         &window );
		
		if ( err == noErr )
		{
			window_closing( window );
		}
		
		return err;
	}
	
	DEFINE_CARBON_UPP( EventHandler, window_WindowClosed )
	
	static EventTypeSpec windowClosed_event[] =
	{
		{ kEventClassWindow, kEventWindowClosed },
	};
	
	
	static
	pascal OSStatus window_ControlDraw( EventHandlerCallRef  handler,
	                                    EventRef             event,
	                                    void*                userData )
	{
		OSStatus err;
		
		ControlRef control;
		err = GetEventParameter( event,
		                         kEventParamDirectObject,
		                         typeControlRef,
		                         NULL,
		                         sizeof (ControlRef),
		                         NULL,
		                         &control );
		
		CGContextRef context = NULL;
		err = GetEventParameter( event,
		                         kEventParamCGContextRef,
		                         typeCGContextRef,
		                         NULL,
		                         sizeof (CGContextRef),
		                         NULL,
		                         &context );
		
	#ifdef MAC_OS_X_VERSION_10_2
		
		HIRect bounds;
		HIViewGetBounds( control, &bounds );
		
		CGContextSetGrayFillColor( context, 1, 1 );  // white
		
		CGContextFillRect( context, bounds );  // erase
		
		CGContextSetGrayFillColor( context, 0, 1 );  // black
		
		if ( View* view = get_window_view( GetControlOwner( control ) ) )
		{
			view->DrawInContext( context, bounds );
		}
		
	#endif
		
		return noErr;
	}
	
	DEFINE_CARBON_UPP( EventHandler, window_ControlDraw )
	
	static EventTypeSpec controlDraw_event[] =
	{
		{ kEventClassControl, kEventControlDraw },
	};
	
	
	OSStatus install_window_event_handlers( WindowRef window )
	{
		OSStatus err;
		
		err = InstallWindowEventHandler( window,
		                                 UPP_ARG( window_WindowClose ),
		                                 1,
		                                 windowClose_event,
		                                 NULL,
		                                 NULL );
		
		err = InstallWindowEventHandler( window,
		                                 UPP_ARG( window_WindowClosed ),
		                                 1,
		                                 windowClosed_event,
		                                 NULL,
		                                 NULL );
		
	#ifdef MAC_OS_X_VERSION_10_2
		
		HIViewRef root = HIViewGetRoot( window );
		
		HIViewRef content;
		err = HIViewFindByID( root, kHIViewWindowContentID, &content );
		
		err = InstallControlEventHandler( content,
		                                  UPP_ARG( window_ControlDraw ),
		                                  1,
		                                  controlDraw_event,
		                                  NULL,
		                                  NULL );
		
	#endif
		
		return err;
	}
	
}
