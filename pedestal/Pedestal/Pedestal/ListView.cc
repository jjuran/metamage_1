/*	===========
 *	ListView.cc
 *	===========
 */

#include "Pedestal/ListView.hh"


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
	
	static Rect AdjustListBounds( const Rect& bounds, bool scrollHoriz, bool scrollVert )
	{
		Rect result = bounds;
		
		AdjustListBounds( result.right, result.bottom, scrollHoriz, scrollVert );
		
		return result;
	}
	
	void ListView::Install( const Rect& bounds )
	{
		const bool drawIt      = true;
		const bool hasGrow     = true;
		const bool scrollHoriz = false;
		const bool scrollVert  = true;
		
		itsList = N::LNew( AdjustListBounds( bounds, false, true ),
		                   N::SetRect( 0, 0, 1, 0 ),  // one column, zero rows
		                   N::SetPt( 0, 0 ),
		                   N::ResID( 0 ),
		                   N::GetWindowFromPort( N::GetQDGlobalsThePort() ),
		                   drawIt,
		                   hasGrow,
		                   scrollHoriz,
		                   scrollVert );
	}
	
	void ListView::Uninstall()
	{
		itsList.reset();
	}
	
	void ListView::SetBounds( const Rect& bounds )
	{
		Rect r = bounds;
		
		AdjustListBounds( r.right, r.bottom, false, true );
		
		N::SetListViewBounds( itsList, r );
		
		N::LSize( r.right - r.left,
		          r.bottom - r.top,
		          itsList );
		
		itsList.Get()[0]->cellSize.h = r.right - r.left;
	}
	
	void ListView::MouseDown( const EventRecord& event )
	{
		N::LClick( N::GlobalToLocal( event.where ),
		           N::EventModifiers( event.modifiers ),
		           itsList );
	}
	
	bool ListView::KeyDown( const EventRecord& event )
	{
		// FIXME
		return false;
	}
	
	void ListView::Draw( const Rect& bounds, bool erasing )
	{
		//Rect bounds = Bounds( itsList );
		//N::EraseRect( bounds );
		N::LUpdate( N::GetPortVisibleRegion( N::GetQDGlobalsThePort() ), itsList );
	}
	
	void ListView::SetCell( UInt16 offset, const char* data, std::size_t length )
	{
		Rect bounds = N::GetListDataBounds( itsList );
		
		if ( offset >= bounds.bottom )
		{
			const UInt16 n_new_rows = offset - bounds.bottom + 1;
			
			LAddRow( n_new_rows, bounds.bottom, itsList );
		}
		
		LSetCell( data, length, N::SetPt( 0, offset ), itsList );
	}
	
	void ListView::AppendCell( const char* data, std::size_t length )
	{
		Rect bounds = N::GetListDataBounds( itsList );
		
		const short i_row = LAddRow( 1, bounds.bottom, itsList );
		
		LSetCell( data, length, N::SetPt( 0, i_row ), itsList );
	}
	
	void ListView::DeleteCells()
	{
		LDelRow( 0, 0, itsList );
	}
	
}

