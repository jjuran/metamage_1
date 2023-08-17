/*	===========
 *	ListView.cc
 *	===========
 */

#include "Pedestal/ListView.hh"

// Nostalgia
#include "Nostalgia/MacWindows.hh"

// mac-qd-utils
#include "mac_qd/get_visRgn.hh"
#include "mac_qd/globals/thePort.hh"
#include "mac_qd/globals/thePort_window.hh"

// Nitrogen
#include "Nitrogen/Events.hh"
#include "Nitrogen/Lists.hh"
#include "Nitrogen/Quickdraw.hh"


namespace Nitrogen
{
	
	inline void SetListViewBounds( ListHandle list, const Rect& bounds )
	{
		::SetListViewBounds( list, &bounds );
	}
	
}

namespace Pedestal
{
	
	namespace N = Nitrogen;
	
	
	// Mac OS places the scrollbars outside the bounds.
	// We adjust the bounds inward so they draw within the original bounds.
	
	static void AdjustListBounds( short& right, short& bottom, bool scrollHoriz, bool scrollVert )
	{
		if ( scrollHoriz )
		{
			bottom -= 15;
		}
		
		if ( scrollVert )
		{
			right -= 15;
		}
	}
	
	static Rect AdjustedListBounds( const Rect&  bounds,
	                                bool         scrollHoriz,
	                                bool         scrollVert )
	{
		Rect result = bounds;
		
		AdjustListBounds( result.right, result.bottom, scrollHoriz, scrollVert );
		
		return result;
	}
	
	static void FixListScrollbarBounds( ListHandle list )
	{
		//bool hasGrow     = GetListFlags( list ) & 0x20;
		const bool hasGrow = true;
		
		const bool scrollVert  = ::GetListVerticalScrollBar  ( list );
		const bool scrollHoriz = ::GetListHorizontalScrollBar( list );
		
		// List Manager bug:  LNew() and LSize() ignore hasGrow, so we have to fudge it
		if ( hasGrow && ( scrollVert != scrollHoriz ) )
		{
			// This hack is only necessary with hasGrow and one scrollbar
			
			ControlRef scrollbar = scrollVert ? ::GetListVerticalScrollBar  ( list )
			                                  : ::GetListHorizontalScrollBar( list );
			
			Rect bounds;
			GetControlBounds( scrollbar, &bounds );
			
			InvalRect(& bounds );
			
			(scrollVert ? bounds.bottom : bounds.right) -= 15;
			
			SetControlBounds( scrollbar, &bounds );
			
			DrawOneControl( scrollbar );
		}
	}
	
	void ListView::Install( const Rect& bounds )
	{
		const bool drawIt      = true;
		const bool hasGrow     = IntersectsGrowBox();
		const bool scrollHoriz = false;
		const bool scrollVert  = true;
		
		const Rect dataBounds = { 0, 0, 0, 1 };  // one column, zero rows
		const Point cSize = {};
		
		itsList = N::LNew( AdjustedListBounds( bounds, scrollHoriz, scrollVert ),
		                   dataBounds,
		                   cSize,
		                   Mac::ResID( 0 ),
		                   mac::qd::thePort_window(),
		                   drawIt,
		                   hasGrow,
		                   scrollHoriz,
		                   scrollVert );
		
		if ( hasGrow )
		{
			FixListScrollbarBounds( itsList );
		}
	}
	
	void ListView::Uninstall()
	{
		itsList.reset();
	}
	
	void ListView::SetBounds( const Rect& bounds )
	{
		const Rect r = AdjustedListBounds( bounds, false, true );
		
		N::SetListViewBounds( itsList, r );
		
		N::LSize( r.right - r.left,
		          r.bottom - r.top,
		          itsList );
		
		itsList.get()[0]->cellSize.h = r.right - r.left;
		
		if ( IntersectsGrowBox() )
		{
			FixListScrollbarBounds( itsList );
		}
	}
	
	bool ListView::MouseDown( const EventRecord& event )
	{
		N::LClick( N::GlobalToLocal( event.where ),
		           N::EventModifiers( event.modifiers ),
		           itsList );
		
		return true;
	}
	
	bool ListView::KeyDown( const EventRecord& event )
	{
		// FIXME
		return false;
	}
	
	void ListView::Draw( const Rect& bounds, bool erasing )
	{
		using mac::qd::get_visRgn;
		using mac::qd::thePort;
		
		//Rect bounds = Bounds( itsList );
		//N::EraseRect( bounds );
		LUpdate( get_visRgn( thePort() ), itsList );
	}
	
	void ListView::SetCell( UInt16 offset, const char* data, std::size_t length )
	{
		Rect bounds = N::GetListDataBounds( itsList );
		
		if ( offset >= bounds.bottom )
		{
			const UInt16 n_new_rows = offset - bounds.bottom + 1;
			
			LAddRow( n_new_rows, bounds.bottom, itsList );
		}
		
		const Point loc = { offset, 0 };
		
		LSetCell( data, length, loc, itsList );
	}
	
	void ListView::AppendCell( const char* data, std::size_t length )
	{
		Rect bounds = N::GetListDataBounds( itsList );
		
		const short i_row = LAddRow( 1, bounds.bottom, itsList );
		
		const Point loc = { i_row, 0 };
		
		LSetCell( data, length, loc, itsList );
	}
	
	void ListView::DeleteCells()
	{
		LDelRow( 0, 0, itsList );
	}
	
}
