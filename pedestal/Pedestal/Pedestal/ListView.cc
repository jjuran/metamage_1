/*	===========
 *	ListView.cc
 *	===========
 */

#include "Pedestal/ListView.hh"

// Universal Interfaces
#include <Fonts.h>

// Standard C++
#include <algorithm>

// Nitrogen
#include "Nitrogen/MacWindows.h"

// Nitrogen Extras / Utilities
#include "Utilities/RectangleMath.h"


namespace Pedestal
{
	
	namespace N = Nitrogen;
	namespace NX = NitrogenExtras;
	
	
	Rect Bounds( ListHandle list )
	{
		return N::GetListViewBounds( list );
	}
	
	Point ViewableRange( ListHandle list )
	{
		return NX::RectSize( N::GetListVisibleCells( list ) );
	}
	
	Point ScrollableRange( ListHandle list )
	{
		return NX::RectSize( N::GetListDataBounds( list ) );
	}
	
	Point ScrollStep( ListHandle list )
	{
		return N::SetPt( 1, 1 );
	}
	
	Point ScrollPosition( ListHandle list )
	{
		return NX::RectPosition( N::GetListVisibleCells( list ) );
	}
	
	ListView::TextAttributes::TextAttributes()
	{
		::TextFont( kFontIDMonaco );
		::TextSize( 9 );
	}
	
	static Rect AdjustListBounds( const Rect& bounds, bool scrollHoriz, bool scrollVert )
	{
		Rect result = bounds;
		
		if ( scrollHoriz )
		{
			result.bottom -= 15;
		}
		
		if ( scrollVert )
		{
			result.right -= 15;
		}
		
		return result;
	}
	
	ListView::ListView( const Rect& bounds )
	:
		list( N::LNew( AdjustListBounds( bounds, false, true ),
		               N::SetRect( 0, 0, 1, 0 ),  // one column, zero rows
		               N::SetPt( 0, 0 ),
		               N::ResID( 0 ),
		               N::GetWindowFromPort( N::GetQDGlobalsThePort() ),
		               true,   // drawIt
		               true,   // hasGrow
		               false,  // scrollHoriz
		               true    // scrollVert
		               ) )
	{
		
	}
	
	void ListView::MouseDown( const EventRecord& event )
	{
		N::LClick( N::GlobalToLocal( event.where ),
		           N::EventModifiers( event.modifiers ),
		           list );
	}
	
	bool ListView::KeyDown( const EventRecord& event )
	{
		// FIXME
		return false;
	}
	
	void ListView::Update()
	{
		//Rect bounds = Bounds( list );
		//N::EraseRect( bounds );
		N::LUpdate( N::GetPortVisibleRegion( N::GetQDGlobalsThePort() ), list );
	}
	
	void ListView::Resize( const Rect& newBounds )
	{
		Rect bounds = AdjustListBounds( newBounds, false, true );
		
		short width  = bounds.right - bounds.left;
		short height = bounds.bottom - bounds.top;
		
		N::LSize( width, height, list );
	}
	
}

