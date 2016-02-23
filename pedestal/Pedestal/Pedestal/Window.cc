/*
	Window.cc
	---------
*/

#include "Pedestal/Window.hh"

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


#if TARGET_API_MAC_CARBON
#define NONCARBON_INLINE  /**/
#else
#define NONCARBON_INLINE  inline
#endif


namespace Pedestal
{
	
	namespace n = nucleus;
	namespace N = Nitrogen;
	
	
	static NONCARBON_INLINE void ResizedWindow( WindowRef window );
	
	static
	void ResizeWindow( WindowRef window, Point newSize )
	{
		N::SizeWindow( window, newSize.h, newSize.v, true );
		
		ResizedWindow( window );
	}
	
	static NONCARBON_INLINE
	void ResizedWindow( WindowRef window )
	{
		// Don't rely on the requested size because it might have been tweaked
		Rect bounds = mac::qd::get_portRect( GetWindowPort( window ) );
		
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
	
	void ResizingWindow( WindowRef window, Point start )
	{
		if ( TARGET_API_MAC_CARBON )
		{
			if ( bool changed = ::ResizeWindow( window, start, NULL, NULL ) )
			{
				if ( GetWindowKind( window ) == kApplicationWindowKind )
				{
					ResizedWindow( window );
				}
			}
			
			return;
		}
		
		Rect sizeRect = { 30, 50, 10000, 10000 };
		
		Point grown = N::GrowWindow( window, start, sizeRect );
		
		if ( grown.h != 0  ||  grown.v != 0 )
		{
			if ( N::GetWindowKind( window ) == N::kApplicationWindowKind )
			{
				ResizeWindow( window, grown );
			}
		}
	}
	
	void SetWindowSize( WindowRef window, Point size )
	{
		n::saved< N::Port > savePort;
		
		N::SetPortWindowPort( window );
		
		ResizeWindow( window, size );
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
