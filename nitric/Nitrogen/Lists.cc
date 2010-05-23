// Nitrogen/Lists.cc
// -----------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2004 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#include "Nitrogen/Lists.hh"

// Nitrogen
#include "Nitrogen/Controls.hh"


namespace Nitrogen
{
	
	nucleus::owned< ListHandle > LNew( const Rect&        rView,
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
		
		return nucleus::owned< ListHandle >::seize( list );
	}
	
	void LSize( short width, short height, ListHandle list )
	{
		::LSize( width, height, list );
	}
	
	nucleus::string LGetCell( Cell cell, ListHandle lHandle )
	{
		nucleus::mutable_string s;
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
	
	Rect GetListViewBounds( ListHandle list )
	{
		Rect result;
		::GetListViewBounds( list, &result );
		
		return result;
	}
	
	ListBounds GetListVisibleCells( ListHandle list )
	{
		ListBounds result;
		::GetListVisibleCells( list, &result );
		
		return result;
	}
	
	ListBounds GetListDataBounds( ListHandle list )
	{
		ListBounds result;
		::GetListDataBounds( list, &result );
		
		return result;
	}
	
	void RegisterListManagerErrors()
	{
		
	}
	
}

