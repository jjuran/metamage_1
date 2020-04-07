/*
	WindowStorage.cc
	----------------
*/

#include "Pedestal/WindowStorage.hh"

// Mac OS
#ifndef __MACWINDOWS__
#include <MacWindows.h>
#endif

// mac-config
#include "mac_config/desk-accessories.hh"

// mac-qd-utils
#include "mac_qd/get_portRect.hh"

// mac-app-utils
#include "mac_app/DAs.hh"

// MacGlue
#include "MacGlue/MacGlue.hh"

// Nitrogen
#include "Mac/Toolbox/Utilities/ThrowOSStatus.hh"

#include "Nitrogen/MacWindows.hh"
#include "Nitrogen/Quickdraw.hh"

// MacFeatures
#include "MacFeatures/ColorQuickdraw.hh"

// Pedestal
#include "Pedestal/Application.hh"
#include "Pedestal/View.hh"
#include "Pedestal/WindowEventHandlers.hh"  // codependent


namespace MacGlue
{
	
	DECLARE_MAC_GLUE( NewWindow  );
	DECLARE_MAC_GLUE( NewCWindow );
	
}

namespace Pedestal
{
	
	namespace n = nucleus;
	namespace N = Nitrogen;
	
	enum
	{
		kWindowCreator = ':-)\xCA',  // Yes, I actually registered this
		
		kWindowViewTag  = 'View',  // Address of view object, if any
		
		kWindowClosedProcTag  = 'Clsd',
		kWindowResizedProcTag = 'Rszd',
	};
	
#if ! OPAQUE_TOOLBOX_STRUCTS
	
	struct WindowStorage
	{
		View*               view;
		WindowClosed_proc   closed_proc;
		WindowResized_proc  resized_proc;
		WindowAttributes    attributes;
		
		WindowRecord        window;
		
		WindowStorage() : view(), closed_proc(), resized_proc()
		{
		}
	};
	
	static
	WindowStorage* RecoverWindowStorage( WindowRef window )
	{
		if ( GetWindowKind( window ) != kApplicationWindowKind )
		{
			return NULL;
		}
		
		void* address = (char*) window - offsetof( WindowStorage, window );
		
		return (WindowStorage*) address;
	}
	
	static
	pascal void DestroyWindow( WindowRef window )
	{
		window_closing( window );
		
		WindowStorage* storage = RecoverWindowStorage( window );
		
		CloseWindow( window );
		
		delete storage;
	}
	
#endif
	
	WindowAttributes get_window_attributes( WindowRef window )
	{
	#if ! TARGET_API_MAC_CARBON
		
		if ( WindowStorage* storage = RecoverWindowStorage( window ) )
		{
			return storage->attributes;
		}
		
		return kWindowNoAttributes;
		
	#endif
		
		OSStatus err;
		
		WindowAttributes attrs = kWindowNoAttributes;
		err = GetWindowAttributes( window, &attrs );
		
		return attrs;
	}
	
	void set_window_view( WindowRef window, View* view )
	{
	#if ! TARGET_API_MAC_CARBON
		
		if ( WindowStorage* storage = RecoverWindowStorage( window ) )
		{
			View* old_view = storage->view;
			
			storage->view = view;
			
			if ( view )
			{
				intrusive_ptr_add_ref( view );
			}
			
			if ( old_view )
			{
				intrusive_ptr_release( old_view );
			}
		}
		
		return;
		
	#endif
		
		OSStatus err;
		
		View* old_view = get_window_view( window );
		
		err = SetWindowProperty( window,
		                         kWindowCreator,
		                         kWindowViewTag,
		                         sizeof view,
		                         &view );
		
		if ( err != noErr  &&  view != NULL )
		{
			Mac::ThrowOSStatus( err );
		}
		
		if ( view )
		{
			intrusive_ptr_add_ref( view );
		}
		
		if ( old_view )
		{
			intrusive_ptr_release( old_view );
		}
	}
	
	View* get_window_view( WindowRef window )
	{
	#if ! TARGET_API_MAC_CARBON
		
		if ( WindowStorage* storage = RecoverWindowStorage( window ) )
		{
			return storage->view;
		}
		
		return NULL;
		
	#endif
		
		OSStatus err;
		
		View* result = NULL;
		err = GetWindowProperty( window,
		                         kWindowCreator,
		                         kWindowViewTag,
		                         sizeof result,
		                         NULL,
		                         &result );
		
		return result;
	}
	
	void set_window_closed_proc( WindowRef          window,
	                             WindowClosed_proc  proc )
	{
	#if ! TARGET_API_MAC_CARBON
		
		if ( WindowStorage* storage = RecoverWindowStorage( window ) )
		{
			storage->closed_proc = proc;
		}
		
		return;
		
	#endif
		
		OSStatus err;
		
		err = SetWindowProperty( window,
		                         kWindowCreator,
		                         kWindowClosedProcTag,
		                         sizeof proc,
		                         &proc );
		
		Mac::ThrowOSStatus( err );
	}
	
	static
	WindowClosed_proc get_window_closed_proc( WindowRef window )
	{
	#if ! TARGET_API_MAC_CARBON
		
		if ( WindowStorage* storage = RecoverWindowStorage( window ) )
		{
			return storage->closed_proc;
		}
		
		return NULL;
		
	#endif
		
		OSStatus err;
		
		WindowClosed_proc result = NULL;
		err = GetWindowProperty( window,
		                         kWindowCreator,
		                         kWindowClosedProcTag,
		                         sizeof result,
		                         NULL,
		                         &result );
		
		return result;
	}
	
	void set_window_resized_proc( WindowRef           window,
	                              WindowResized_proc  proc )
	{
	#if ! TARGET_API_MAC_CARBON
		
		if ( WindowStorage* storage = RecoverWindowStorage( window ) )
		{
			storage->resized_proc = proc;
		}
		
		return;
		
	#endif
		
		OSStatus err;
		
		err = SetWindowProperty( window,
		                         kWindowCreator,
		                         kWindowResizedProcTag,
		                         sizeof proc,
		                         &proc );
		
		Mac::ThrowOSStatus( err );
	}
	
	WindowResized_proc get_window_resized_proc( WindowRef window )
	{
	#if ! TARGET_API_MAC_CARBON
		
		if ( WindowStorage* storage = RecoverWindowStorage( window ) )
		{
			return storage->resized_proc;
		}
		
		return NULL;
		
	#endif
		
		OSStatus err;
		
		WindowResized_proc result = NULL;
		err = GetWindowProperty( window,
		                         kWindowCreator,
		                         kWindowResizedProcTag,
		                         sizeof result,
		                         NULL,
		                         &result );
		
		return result;
	}
	
	void invalidate_window( WindowRef window )
	{
	#ifdef MAC_OS_X_VERSION_10_2
		
		if ( get_window_attributes( window ) & kWindowCompositingAttribute )
		{
			OSStatus err;
			
			ControlRef content;
			err = GetRootControl( window, &content );
			
			err = HIViewSetNeedsDisplay( content, true );
			
			return;
		}
		
	#endif
		
		CGrafPtr port = GetWindowPort( window );
		
		if ( IsPortVisibleRegionEmpty( port ) )
		{
			return;
		}
		
		N::InvalWindowRect( window, mac::qd::get_portRect( port ) );
		
		ScheduleImmediateEventCheck();
	}
	
	void close_window( WindowRef window )
	{
		if ( WindowClosed_proc proc = get_window_closed_proc( window ) )
		{
			proc( window );
		}
		else
		{
		#if ! OPAQUE_TOOLBOX_STRUCTS
			
			DestroyWindow( window );
			
		#else
			
			DisposeWindow( window );
			
		#endif
		}
	}
	
	void window_closing( WindowRef window )
	{
		if ( CONFIG_DESK_ACCESSORIES  &&  mac::app::close_DA_window( window ) )
		{
			return;
		}
		
		set_window_view( window, NULL );
	}
	
	
	typedef pascal WindowRef (*NewWindow_ProcPtr)( void*             storage,
	                                               const Rect*       bounds,
	                                               ConstStr255Param  title,
	                                               Boolean           visible,
	                                               short             procID,
	                                               WindowRef         behind,
	                                               Boolean           goAwayFlag,
	                                               long              refCon );
	
	using MacFeatures::Has_ColorQuickdraw;
	
	static const NewWindow_ProcPtr gNewWindow = Has_ColorQuickdraw() ? &MacGlue::NewCWindow
	                                                                 : &MacGlue::NewWindow;
	
	
	static inline
	bool has_grow_box( short procID )
	{
		// This works for the standard WDEF.
		
		return (procID & ~0x8) == 0;  // documentProc (0) or zoomDocProc (8)
	}
	
	static inline
	bool has_zoom_box( short procID )
	{
		// This works for the standard WDEF.
		
		return (procID & ~0x4) == 8;  // zoomDocProc (8) or zoomNoGrow (12)
	}
	
	typedef nucleus::disposer_class< WindowRef >::type Disposer;
	
	static inline
	n::owned< WindowRef > finish_window( WindowRef  window,
	                                     Disposer   disposer )
	{
		SetPortWindowPort( window );
		
		n::owned< WindowRef > result =
		n::owned< WindowRef >::seize( window, disposer );
		
		if ( TARGET_API_MAC_CARBON )
		{
			OSStatus err = install_window_event_handlers( window );
			
			Mac::ThrowOSStatus( err );
		}
		
		return result;
	}
	
	n::owned< WindowRef > CreateWindow( const Rect&           bounds,
	                                    ConstStr255Param      title,
	                                    bool                  visible,
	                                    Mac::WindowDefProcID  procID,
	                                    bool                  goAwayFlag )
	{
		Disposer disposer;
		
		WindowRef substorage = NULL;
		
	#if ! TARGET_API_MAC_CARBON
		
		disposer = &DestroyWindow;
		
		WindowStorage* storage = new WindowStorage;
		
		storage->attributes = kWindowResizableAttribute * has_grow_box( procID )
		                    | kWindowFullZoomAttribute  * has_zoom_box( procID )
		                    | kWindowCloseBoxAttribute  * goAwayFlag;
		
		substorage = &storage->window.port;
		
	#endif
		
		WindowRef window = gNewWindow( substorage,
		                               &bounds,
		                               title,
		                               visible,
		                               procID,
		                               kFirstWindowOfClass,
		                               goAwayFlag,
		                               0 );
		
		if ( window == NULL )
		{
			OSStatus err = MemError();
			
			if ( err == noErr )
			{
				err = paramErr;  // E.g. invalid procID
			}
			
		#if ! TARGET_API_MAC_CARBON
			
			delete storage;
			
		#endif
			
			Mac::ThrowOSStatus( err );
		}
		
		return finish_window( window, disposer );
	}
	
	n::owned< WindowRef > CreateWindow( Mac::WindowClass       wClass,
	                                    Mac::WindowAttributes  attrs,
	                                    const Rect&            bounds )
	{
		OSStatus err;
		
		WindowRef window;
		err = CreateNewWindow( wClass, attrs, &bounds, &window );
		
		Mac::ThrowOSStatus( err );
		
		return finish_window( window, Disposer() );
	}
	
}
