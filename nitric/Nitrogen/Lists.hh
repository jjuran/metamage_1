// Nitrogen/Lists.hh
// -----------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2004 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef NITROGEN_LISTS_HH
#define NITROGEN_LISTS_HH

#ifndef __LISTS__
#include <Lists.h>
#endif

// iota
#include "iota/string_traits.hh"

// nucleus
#include "nucleus/owned.hh"
#include "nucleus/string.hh"

// Nitrogen
#include "Mac/Lists/Types/ListHandle.hh"
#include "Mac/Resources/Types/ResID.hh"

#ifndef NITROGEN_EVENTS_HH
#include "Nitrogen/Events.hh"
#endif


namespace Nitrogen
{
	
	void RegisterListManagerErrors();
	
	using ::Cell;
	using ::ListBounds;
	
	using ::ListRec;
	using ::ListPtr;
	using ::ListHandle;
	
	// 436
	nucleus::owned< ListHandle > LNew( const Rect&        rView,
	                                   const ListBounds&  dataBounds,
	                                   Point              cSize,
	                                   Mac::ResID         theProc,
	                                   WindowRef          theWindow,
	                                   bool               drawIt,
	                                   bool               hasGrow,
	                                   bool               scrollHoriz,
	                                   bool               scrollVert );
	
	// 457
	inline void LDispose( nucleus::owned< ListHandle > )  {}
	
	// 469, 484, 499, 514
	using ::LAddColumn;
	using ::LAddRow;
	using ::LDelColumn;
	using ::LDelRow;
	
	// 529
	inline bool LGetSelect( bool next, Cell& cell, ListHandle lHandle )
	{
		return ::LGetSelect( next, &cell, lHandle );
	}
	
	// 544
	using ::LLastClick;
	
	// LNextCell()
	// LSearch()
	
	// 589
	void LSize( short width, short height, ListHandle list );
	
	// 604
	inline void LSetDrawingMode( bool drawIt, ListHandle lHandle )
	{
		::LSetDrawingMode( drawIt, lHandle );
	}
	
	// 618, 633, 645
	using ::LScroll;
	using ::LAutoScroll;
	using ::LUpdate;
	
	// 659
	inline void LActivate( bool act, ListHandle lHandle )  { ::LActivate( act, lHandle ); }
	
	// 673
	using ::LCellSize;
	
	// 687
	inline bool LClick( Point pt, EventModifiers modifiers, ListHandle lHandle )
	{
		return ::LClick( pt, modifiers, lHandle );
	}
	
	// 702
	using ::LAddToCell;
	
	template < class String >
	inline void LAddToCell( const String& data, Cell cell, ListHandle lHandle )
	{
		::LAddToCell( iota::get_string_data( data ),
		              iota::get_string_size( data ),
		              cell,
		              lHandle );
	}
	
	// 718
	using ::LClrCell;
	
	// 732
	nucleus::string LGetCell( Cell cell, ListHandle lHandle );
	
	// 748
	Rect LRect( Cell cell, ListHandle lHandle );
	
	// 763
	using ::LSetCell;
	
	template < class String >
	inline void LSetCell( const String& data, Cell cell, ListHandle lHandle )
	{
		::LSetCell( iota::get_string_data( data ),
		            iota::get_string_size( data ),
		            cell,
		            lHandle );
	}
	
	// 779
	inline void LSetSelect( bool setIt, Cell cell, ListHandle lHandle )
	{
		::LSetSelect( setIt, cell, lHandle );
	}
	
	// 794
	using ::LDraw;
	
	struct LGetCellDataLocation_Result
	{
		short offset;
		short len;
	};
	
	// 808
	LGetCellDataLocation_Result LGetCellDataLocation( Cell cell, ListHandle lHandle );
	
	// 1089
	Rect GetListViewBounds( ListHandle list );
	
	// 1143
	ListBounds GetListVisibleCells( ListHandle list );
	
	// 1281
	ListBounds GetListDataBounds( ListHandle list );
	
}

#endif

