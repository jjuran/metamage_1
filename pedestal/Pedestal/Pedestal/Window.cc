/*	============
 *	PedWindow.cc
 *	============
 */

#include "Pedestal/Window.hh"

// Nucleus
#include "Nucleus/Saved.h"

// ClassicToolbox
#include "ClassicToolbox/MacWindows.h"


namespace Pedestal
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	
	
	void ResizeWindow( N::WindowRef window, Point newSize )
	{
		if ( WindowBase* base = N::GetWRefCon( window ) )
		{
			// Resize the window
			base->Resize( window, newSize.h, newSize.v );
			
			// Don't rely on the requested size because it might have been tweaked
			Rect bounds = N::GetPortBounds( N::GetWindowPort( window ) );
			
			// Inform the window's contents that it has been resized
			base->Resized( bounds.right, bounds.bottom );
		}
	}
	
	
	Point GetWindowSize( N::WindowRef window )
	{
		Rect bounds = N::GetPortBounds( N::GetWindowPort( window ) );
		
		Point result;
		
		result.h = bounds.right - bounds.left;
		result.v = bounds.bottom - bounds.top;
		
		return result;
	}
	
	Point GetWindowPosition( N::WindowRef window )
	{
		NN::Saved< N::Port_Value > savedPort;
		
		N::SetPortWindowPort( window );
		
		Point upperLeft = { 0, 0 };
		
		::LocalToGlobal( &upperLeft );
		
		return upperLeft;
	}
	
	
	NN::Owned< N::WindowRef > CreateWindow( const Rect&         bounds,
	                                        ConstStr255Param    title,
	                                        bool                visible,
	                                        N::WindowDefProcID  procID,
	                                        N::WindowRef        behind,
	                                        bool                goAwayFlag,
	                                        N::RefCon           refCon )
	{
		NN::Owned< N::WindowRef > window = N::NewCWindow( bounds,
		                                                  title,
		                                                  visible,
		                                                  procID,
		                                                  behind,
		                                                  goAwayFlag,
		                                                  refCon );
		
		//N::SetWindowKind( window, kPedestalWindowKind );
		N::SetPortWindowPort( window );
		
		return window;
	}
	
	static Rect GrowBoxBounds( N::WindowRef window )
	{
		Rect bounds = N::GetPortBounds( N::GetWindowPort( window ) );
		
		bounds.left = bounds.right - 15;
		bounds.top = bounds.bottom - 15;
		
		return bounds;
	}
	
	void DrawWindow( N::WindowRef window )
	{
		NN::Saved< N::Clip_Value > savedClip;
		
		N::ClipRect( GrowBoxBounds( window ) );
		
		N::DrawGrowIcon( window );
	}
	
	void InvalidateWindowGrowBox( N::WindowRef window )
	{
		N::InvalRect( GrowBoxBounds( window ) );
	}
	
	
	ClosableWindow::~ClosableWindow()
	{
	}
	
	ResizableWindow::~ResizableWindow()
	{
	}
	
	void ResizableWindow::Resize( Nitrogen::WindowRef window, short h, short v )
	{
		InvalidateWindowGrowBox( window );  // assume grow box present on resize
		
		if ( const WindowResizeHandler* handler = itsResizeHandler.get() )
		{
			(*handler)( window, h, v );
		}
		else
		{
			Nitrogen::SizeWindow( window, h, v, true );
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

