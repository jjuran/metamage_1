// Lists.h

#ifndef NITROGEN_LISTS_H
#define NITROGEN_LISTS_H

#include <TargetConditionals.h>
#if TARGET_RT_MAC_MACHO

#else
	#include <Lists.h>
#endif

// Nitrogen
#ifndef NITROGEN_EVENTS_H
#include "Nitrogen/Events.h"
#endif
#ifndef NITROGEN_OWNED_H
#include "Nitrogen/Owned.h"
#endif
#ifndef NITROGEN_RESOURCES_H
#include "Nitrogen/Resources.h"
#endif


namespace Nitrogen
{
	
	void RegisterListManagerErrors();
	
	using ::Cell;
	using ::ListBounds;
	
	using ::ListRec;
	using ::ListPtr;
	using ::ListHandle;
	
	template <> struct Disposer< ListHandle > : public std::unary_function< ListHandle, void >
	{
		void operator()( ListHandle list ) const
		{
			::LDispose( list );
		}
	};
	
	
	// 436
	Owned< ListHandle > LNew( const Rect&        rView,
	                          const ListBounds&  dataBounds,
	                          Point              cSize,
	                          ResID              theProc,
	                          WindowRef          theWindow,
	                          bool               drawIt,
	                          bool               hasGrow,
	                          bool               scrollHoriz,
	                          bool               scrollVert );
	
	// 457
	inline void LDispose( Owned< ListHandle > )  {}
	
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
	inline void LAddToCell( const std::string& data, Cell cell, ListHandle lHandle )
	{
		::LAddToCell( data.data(), data.size(), cell, lHandle );
	}
	
	// 718
	using ::LClrCell;
	
	// 732
	std::string LGetCell( Cell cell, ListHandle lHandle );
	
	// 748
	Rect LRect( Cell cell, ListHandle lHandle );
	
	// 763
	using ::LSetCell;
	inline void LSetCell( const std::string& data, Cell cell, ListHandle lHandle )
	{
		::LSetCell( data.data(), data.size(), cell, lHandle );
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
	
}

#endif

