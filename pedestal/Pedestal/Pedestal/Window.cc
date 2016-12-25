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
	
	
	void ResizeWindow( WindowRef window, Point newSize )
	{
		N::SizeWindow( window, newSize.h, newSize.v, true );
		
		// Don't rely on the requested size because it might have been tweaked
		Rect bounds = N::GetPortBounds( N::GetWindowPort( window ) );
		
		// Shotgun approach -- invalidate the whole window.
		// This conveniently includes both old and new grow box locations.
		// Clients can validate regions if they want.
		
		invalidate_window( window );
		
		if ( View* view = get_window_view( window ) )
		{
			view->SetBounds( bounds );
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
		
		set_window_view( itsWindowRef.get(), NULL );
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
