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


namespace Pedestal
{
	
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
		
		const short proc = 0;
		
		const Rect dataBounds = { 0, 0, 0, 1 };  // one column, zero rows
		const Point cSize = {};
		
		Rect rView = AdjustedListBounds( bounds, scrollHoriz, scrollVert );
		
		ListHandle list = ::LNew( &rView,
		                          &dataBounds,
		                          cSize,
		                          proc,
		                          mac::qd::thePort_window(),
		                          drawIt,
		                          hasGrow,
		                          scrollHoriz,
		                          scrollVert );
		
		// FIXME:  Check for null handle
		
		itsList = nucleus::owned< ListHandle >::seize( list );
		
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
		
		SetListViewBounds( itsList, &r );
		
		LSize( r.right - r.left,
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
		Point where = event.where;
		
		GlobalToLocal( &where );
		
		/*
			MWC68K mysteriously generates smaller code
			with the (int) cast than without it.
		*/
		
		LClick( where,
		        (int) event.modifiers,
		        itsList.get() );
		
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
		//EraseRect( &bounds );
		
		LUpdate( get_visRgn( thePort() ), itsList );
	}
	
	void ListView::SetCell( UInt16 offset, const char* data, std::size_t length )
	{
		Rect bounds;
		
		GetListDataBounds( itsList, &bounds );
		
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
		Rect bounds;
		
		GetListDataBounds( itsList, &bounds );
		
		const short i_row = LAddRow( 1, bounds.bottom, itsList );
		
		const Point loc = { i_row, 0 };
		
		LSetCell( data, length, loc, itsList );
	}
	
	void ListView::DeleteCells()
	{
		LDelRow( 0, 0, itsList );
	}
	
}
