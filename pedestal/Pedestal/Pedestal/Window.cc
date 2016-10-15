/*	============
 *	PedWindow.cc
 *	============
 */

#include "Pedestal/Window.hh"

// MacGlue
#include "MacGlue/MacGlue.hh"

// nucleus
#include "nucleus/saved.hh"

// Nitrogen
#include "Nitrogen/Quickdraw.hh"

// ClassicToolbox
#include "ClassicToolbox/MacWindows.hh"

// MacFeatures
#include "MacFeatures/ColorQuickdraw.hh"

// Pedestal
#include "Pedestal/View.hh"
#include "Pedestal/WindowMenu.hh"


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
		
		kWindowOwnerTag = 'This',  // Address of owning object
		
		kWindowClosedProcTag  = 'Clsd',
		kWindowResizedProcTag = 'Rszd',
	};
	
#if ! OPAQUE_TOOLBOX_STRUCTS
	
	struct WindowStorage
	{
		Window*             owner;
		WindowClosed_proc   closed_proc;
		WindowResized_proc  resized_proc;
		WindowAttributes    attributes;
		WindowRecord        window;
		
		WindowStorage() : closed_proc(), resized_proc()
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
	
	static
	void set_window_owner( WindowRef window, Window* owner )
	{
	#if ! TARGET_API_MAC_CARBON
		
		WindowStorage* storage = RecoverWindowStorage( window );
		
		/*
			This is only called on windows created below, which will always
			be application windows with custom storage.
		*/
		
		storage->owner = owner;
		
		return;
		
	#endif
		
		OSStatus err;
		
		err = SetWindowProperty( window,
		                         kWindowCreator,
		                         kWindowOwnerTag,
		                         sizeof owner,
		                         &owner );
		
		Mac::ThrowOSStatus( err );
	}
	
	Window* get_window_owner( WindowRef window )
	{
	#if ! TARGET_API_MAC_CARBON
		
		if ( WindowStorage* storage = RecoverWindowStorage( window ) )
		{
			return storage->owner;
		}
		
		return NULL;
		
	#endif
		
		OSStatus err;
		
		Window* result = NULL;
		err = GetWindowProperty( window,
		                         kWindowCreator,
		                         kWindowOwnerTag,
		                         sizeof result,
		                         NULL,
		                         &result );
		
		return result;
	}
	
	View* get_window_view( WindowRef window )
	{
		if ( Window* owner = get_window_owner( window ) )
		{
			return owner->GetView().get();
		}
		
		return NULL;
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
		                         kWindowOwnerTag,
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
		                         kWindowOwnerTag,
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
		                         kWindowOwnerTag,
		                         kWindowResizedProcTag,
		                         sizeof proc,
		                         &proc );
		
		Mac::ThrowOSStatus( err );
	}
	
	static
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
		                         kWindowOwnerTag,
		                         kWindowResizedProcTag,
		                         sizeof result,
		                         NULL,
		                         &result );
		
		return result;
	}
	
	void close_window( WindowRef window )
	{
		if ( WindowClosed_proc proc = get_window_closed_proc( window ) )
		{
			proc( window );
			
			return;
		}
		
		if ( Window* obj = get_window_owner( window ) )
		{
			delete obj;
		}
		else
		{
			DisposeWindow( window );
		}
	}
	
	static
	bool window_has_grow_icon( WindowRef window )
	{
		return get_window_attributes( window ) & kWindowResizableAttribute;
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
	
	
	static
	Rect GrowBoxBounds( WindowRef window )
	{
		Rect bounds = N::GetPortBounds( N::GetWindowPort( window ) );
		
		bounds.left = bounds.right - 15;
		bounds.top = bounds.bottom - 15;
		
		return bounds;
	}
	
	static
	void InvalidateWindowGrowBox( WindowRef window )
	{
		N::InvalRect( GrowBoxBounds( window ) );
	}
	
	void ResizeWindow( WindowRef window, Point newSize )
	{
		InvalidateWindowGrowBox( window );  // assume grow box present on resize
		
		N::SizeWindow( window, newSize.h, newSize.v, true );
		
		// Don't rely on the requested size because it might have been tweaked
		Rect bounds = N::GetPortBounds( N::GetWindowPort( window ) );
		
		// Shotgun approach -- invalidate the whole window.
		// Clients can validate regions if they want.
		N::InvalRect( bounds );
		
		if ( View* view = get_window_view( window ) )
		{
			view->SetBounds( bounds );
		}
		
		if ( window_has_grow_icon( window ) )
		{
			InvalidateWindowGrowBox( window );
		}
		
		if ( WindowResized_proc proc = get_window_resized_proc( window ) )
		{
			proc( window );
		}
	}
	
	void SetWindowSize( WindowRef window, Point size )
	{
		n::saved< N::Port > savePort;
		
		N::SetPortWindowPort( window );
		
		ResizeWindow( window, size );
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
	
	static
	n::owned< WindowRef > CreateWindow( const Rect&         bounds,
	                                    ConstStr255Param    title,
	                                    bool                visible,
	                                    N::WindowDefProcID  procID,
	                                    WindowRef           behind,
	                                    bool                goAwayFlag )
	{
		typedef nucleus::disposer_class< WindowRef >::type Disposer;
		
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
		                               behind,
		                               goAwayFlag,
		                               0 );
		
		//N::SetWindowKind( window, kPedestalWindowKind );
		N::SetPortWindowPort( window );
		
		return n::owned< WindowRef >::seize( window, disposer );
	}
	
	static inline
	n::owned< WindowRef > CreateWindow( const NewWindowContext& context )
	{
		return CreateWindow( context.bounds,
		                     context.title,
		                     context.visible,
		                     context.procID,
		                     context.behind,
		                     context.goAwayFlag );
	}
	
	
	Window::Window( const NewWindowContext& context )
	:
		itsWindowRef( CreateWindow( context ) )
	{
		set_window_owner( itsWindowRef, this );
		
		window_created( itsWindowRef.get() );
	}
	
	Window::~Window()
	{
		window_removed( itsWindowRef.get() );
	}
	
	
	void window_activated( WindowRef window, bool activating )
	{
		if ( View* view = get_window_view( window ) )
		{
			view->Activate( activating );
		}
		
		if ( window_has_grow_icon( window ) )
		{
			InvalidateWindowGrowBox( window );
		}
	}
	
	void Window::MouseDown( const EventRecord& event )
	{
		// FIXME:  The window may want clicks even if it's not in front.
		if ( Get() != Nitrogen::FrontWindow() )
		{
			Nitrogen::SelectWindow( Get() );
		}
		else
		{
			GetView()->MouseDown( event );
		}
	}
	
	void Window::Update()
	{
		GetView()->Draw( Nitrogen::GetPortBounds( Nitrogen::GetWindowPort( Get() ) ), true );
		
		if ( !TARGET_API_MAC_CARBON && window_has_grow_icon( Get() ) )
		{
			n::saved< N::Clip > savedClip;
			
			N::ClipRect( GrowBoxBounds( Get() ) );
			
			N::DrawGrowIcon( Get() );
		}
	}
	
	
	#if 0
	
	static Rect CalcWindowStructureDiff()
	{
		Rect r = { -200, -200, -100, -100 };
		unsigned char* title = "\pTest";
		bool vis = true;
		int procID = 0;
		WindowRef front = kFirstWindowOfClass;
		bool goAway = true;
		int refCon = 0;
		WindowRef windowPtr = ::NewWindow(NULL, &r, title, vis, procID, front, goAway, refCon);
		
		VRegion region;
		::GetWindowStructureRgn(windowPtr, region);
		::DisposeWindow(windowPtr);
		RgnHandle rgnH = region;
		Rect bounds = (**rgnH).rgnBBox;
		
		return SetRect(
			bounds.left   - r.left, 
			bounds.top    - r.top, 
			bounds.right  - r.right, 
			bounds.bottom - r.bottom
		);
	}
	
	static Rect WindowStructureDiff()
	{
		static Rect diff = CalcWindowStructureDiff();
		return diff;
	}
	
	#endif
	
}
