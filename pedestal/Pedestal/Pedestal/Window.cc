/*	============
 *	PedWindow.cc
 *	============
 */

#include "Pedestal/Window.hh"

// Nitrogen Extras / ClassicToolbox
#include "ClassicToolbox/MacWindows.h"

// Nitrogen Extras / Utilities
#include "Utilities/Quickdraw.h"
#include "Utilities/Saved.h"


namespace Pedestal {
	
	namespace N = Nitrogen;
	
	N::Owned< N::WindowRef > CreateWindow
	(
		const Rect& bounds, 
		ConstStr255Param title, 
		bool visible, 
		N::WindowDefProcID procID, 
		N::WindowRef behind, 
		bool goAwayFlag, 
		N::RefCon refCon
	)
	{
		N::Owned< N::WindowRef > window = N::NewCWindow( bounds, title, visible, procID, behind, goAwayFlag, refCon );
		
		//N::SetWindowKind( window, kPedestalWindowKind );
		N::SetPortWindowPort( window );
		
		return window;
	}
	
	void DrawWindow( N::WindowRef window )
	{
		N::Saved< N::Clip_Value > savedClip;
		
		Rect bounds = N::GetPortBounds( N::GetWindowPort( window ) );
		
		N::ClipRect( N::OffsetRect( N::SetRect( -15, -15, 0, 0 ),
		                            bounds.right,
		                            bounds.bottom ) );
		
		N::DrawGrowIcon( window );
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

