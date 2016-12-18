/*	============
 *	PedWindow.cc
 *	============
 */

#include "Pedestal/Window.hh"

// nucleus
#include "nucleus/saved.hh"

// Nitrogen
#include "Nitrogen/Quickdraw.hh"

// ClassicToolbox
#include "ClassicToolbox/MacWindows.hh"

// Pedestal
#include "Pedestal/View.hh"
#include "Pedestal/WindowEventHandlers.hh"
#include "Pedestal/WindowMenu.hh"
#include "Pedestal/WindowStorage.hh"


namespace Pedestal
{
	
	namespace n = nucleus;
	namespace N = Nitrogen;
	
	static
	bool window_has_grow_icon( WindowRef window )
	{
		return get_window_attributes( window ) & kWindowResizableAttribute;
	}
	
	
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
	
	
	Window::Window( nucleus::owned< WindowRef > window )
	:
		itsWindowRef( window )
	{
		set_window_owner( itsWindowRef, this );
		
		if ( TARGET_API_MAC_CARBON )
		{
			OSStatus err = install_window_event_handlers( itsWindowRef.get() );
			
			Mac::ThrowOSStatus( err );
		}
		
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
		
		if ( ! TARGET_API_MAC_CARBON  &&  window_has_grow_icon( window ) )
		{
			n::saved< N::Clip > savedClip;
			
			N::ClipRect( GrowBoxBounds( window ) );
			
			DrawGrowIcon( window );
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
