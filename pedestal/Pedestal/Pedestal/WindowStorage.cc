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

// mac-sys-utils
#include "mac_sys/mem_error.hh"

// mac-ui-utils
#include "mac_ui/windows.hh"

// mac-app-utils
#include "mac_app/DAs.hh"
#include "mac_app/new_window.hh"
#include "mac_app/state.hh"
#include "mac_app/Window_menu.hh"

// Nitrogen
#include "Mac/Toolbox/Utilities/ThrowOSStatus.hh"

// Pedestal
#include "Pedestal/View.hh"
#include "Pedestal/WindowEventHandlers.hh"  // codependent


namespace Pedestal
{
	
	namespace n = nucleus;
	
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
		mac::ui::invalidate_window( window );
		
		mac::app::event_check_due = true;
	}
	
	void close_window( WindowRef window )
	{
		mac::app::Window_menu_remove( window );
		
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
		
		mac::app::Window_menu_insert( window );
		
		return result;
	}
	
	n::owned< WindowRef > CreateWindow( const Rect&       bounds,
	                                    ConstStr255Param  title,
	                                    bool              visible,
	                                    WindowDefProcID   procID,
	                                    bool              goAwayFlag )
	{
		using mac::app::new_window;
		
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
		
		WindowRef window = new_window( substorage,
		                               &bounds,
		                               title,
		                               visible,
		                               procID,
		                               kFirstWindowOfClass,
		                               goAwayFlag,
		                               0 );
		
		if ( window == NULL )
		{
			OSStatus err = mac::sys::mem_error();
			
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
	
	n::owned< WindowRef > CreateWindow( WindowClass       wClass,
	                                    WindowAttributes  attrs,
	                                    const Rect&       bounds )
	{
		OSStatus err;
		
		WindowRef window;
		err = CreateNewWindow( wClass, attrs, &bounds, &window );
		
		Mac::ThrowOSStatus( err );
		
		return finish_window( window, Disposer() );
	}
	
}
