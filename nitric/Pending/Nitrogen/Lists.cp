// Lists.cp

#ifndef NITROGEN_LISTS_H
#include "Nitrogen/Lists.h"
#endif

#ifndef NITROGEN_CONTROLS_H
#include "Nitrogen/Controls.h"
#endif


namespace Nitrogen
{
	
	static void FixListScrollbarBounds( ListHandle list )
	{
		bool hasGrow = GetListFlags( list ) & 0x20;
		bool scrollVert = GetListVerticalScrollBar( list );
		bool scrollHoriz = GetListHorizontalScrollBar( list );
		
		// List Manager bug:  LNew() and LSize() ignore hasGrow, so we have to fudge it
		if ( hasGrow && ( scrollVert != scrollHoriz ) )
		{
			// This hack is only necessary with hasGrow and one scrollbar
			
			ControlRef scrollbar = NULL;
			Rect bounds;
			
			if ( scrollVert )
			{
				scrollbar = GetListVerticalScrollBar( list );
				bounds = GetControlBounds( scrollbar );
				bounds.bottom -= 15;
				
			}
			else //  if ( scrollHoriz )
			{
				scrollbar = GetListHorizontalScrollBar( list );
				bounds = GetControlBounds( scrollbar );
				bounds.right -= 15;
				SetControlBounds( scrollbar, bounds );
			}
			
			SetControlBounds( scrollbar, bounds );
			DrawOneControl( scrollbar );
		}
	}
	
	Owned< ListHandle > LNew( const Rect&        rView,
	                          const ListBounds&  dataBounds,
	                          Point              cSize,
	                          ResID              theProc,
	                          WindowRef          theWindow,
	                          bool               drawIt,
	                          bool               hasGrow,
	                          bool               scrollHoriz,
	                          bool               scrollVert )
	{
		ListHandle list = ::LNew( &rView,
		                          &dataBounds,
		                          cSize,
		                          theProc,
		                          theWindow,
		                          drawIt,
		                          hasGrow,
		                          scrollHoriz,
		                          scrollVert );
		
		// FIXME:  Check for null handle
		
		FixListScrollbarBounds( list );
		
		return Owned< ListHandle >::Seize( list );
	}
	
	void LSize( short width, short height, ListHandle list )
	{
		::LSize( width, height, list );
		
		FixListScrollbarBounds( list );
	}
	
	std::string LGetCell( Cell cell, ListHandle lHandle )
	{
		std::string s;
		short length = LGetCellDataLocation( cell, lHandle ).len;
		s.resize( length );
		
		
		::LGetCell( &s[0], &length, cell, lHandle );
		
		return s;
	}
	
	Rect LRect( Cell cell, ListHandle lHandle )
	{
		Rect rect;
		::LRect( &rect, cell, lHandle );
		return rect;
	}
	
	LGetCellDataLocation_Result LGetCellDataLocation( Cell cell, ListHandle lHandle )
	{
		LGetCellDataLocation_Result result;
		
		::LGetCellDataLocation( &result.offset, &result.len, cell, lHandle );
		
		return result;
	}
	
	void RegisterListManagerErrors()
	{
		
	}
	
}

