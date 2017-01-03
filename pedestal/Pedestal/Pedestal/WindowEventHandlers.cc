/*
	WindowEventHandlers.cc
	----------------------
*/

#include "Pedestal/WindowEventHandlers.hh"

// Mac OS
#ifndef __CARBONEVENTS__
#include <CarbonEvents.h>
#endif

// nucleus
#include "nucleus/saved.hh"

// Nitrogen
#include "Nitrogen/Quickdraw.hh"

// ClassicToolbox
#include "ClassicToolbox/MacWindows.hh"

// Pedestal
#include "Pedestal/View.hh"
#include "Pedestal/WindowStorage.hh"


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
		Rect bounds = N::GetPortBounds( N::GetWindowPort( window ) );
		
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
			view->Draw( N::GetPortBounds( GetWindowPort( window ) ), true );
		}
		
		if ( window_has_grow_icon( window ) )
		{
			n::saved< N::Clip > savedClip;
			
			N::ClipRect( GrowBoxBounds( window ) );
			
			DrawGrowIcon( window );
		}
	}
	
	
#if ! TARGET_RT_MAC_CFM
	
	/*
		Null definitions for classic 68K and Mach-O.
		(UPPs are plain functions, not routine descriptors.)
	*/
	
	#define DEFINE_UPP2( UPPTypeName, proc )  /**/
	
	#define UPP_ARG( proc )  &proc
	
#else
	
	/*
		CFM:  Create a routine descriptor at static initialization time.
	*/
	
	#define UPP_VARNAME( proc )  proc ## _upp
	
	#define DEFINE_UPP2( UPPTypeName, proc )  \
	static UPPTypeName UPP_VARNAME( proc ) = New ## UPPTypeName( &proc );
	
	#define UPP_ARG( proc )  UPP_VARNAME( proc )
	
#endif
	
	#define DEFINE_UPP( BaseName, proc )  DEFINE_UPP2( BaseName ## UPP, proc )
	
#if ! TARGET_RT_MAC_CFM  ||  TARGET_API_MAC_CARBON
	
	/*
		For non-CFM, use the null macro above.
		For Carbon CFM, create a routine descriptor as usual.
	*/
	
	#define DEFINE_CARBON_UPP( BaseName, proc )  DEFINE_UPP( BaseName, proc )
	
#else
	
	/*
		For non-Carbon CFM, declare the UPP variable extern, but don't
		define it.  Since these are Carbon event handlers, installing them
		would be a run-time error, so make it a link error instead, and don't
		waste resources creating the routine descriptors.
	*/
	
	#define DEFINE_CARBON_UPP( BaseName, proc )  \
	extern BaseName ## UPP UPP_VARNAME( proc );
	
#endif
	
	
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
	
	static EventTypeSpec windowClose_event[] =
	{
		{ kEventClassWindow, kEventWindowClose },
	};
	
	DEFINE_CARBON_UPP( EventHandler, window_WindowClose )
	
	
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
	
	static EventTypeSpec controlDraw_event[] =
	{
		{ kEventClassControl, kEventControlDraw },
	};
	
	DEFINE_CARBON_UPP( EventHandler, window_ControlDraw )
	
	
	OSStatus install_window_event_handlers( WindowRef window )
	{
		OSStatus err;
		
		err = InstallWindowEventHandler( window,
		                                 UPP_ARG( window_WindowClose ),
		                                 1,
		                                 windowClose_event,
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
