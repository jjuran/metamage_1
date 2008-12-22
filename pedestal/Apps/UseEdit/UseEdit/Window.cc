/*	=========
 *	Window.cc
 *	=========
 */

#include "UseEdit/Window.hh"

// Nucleus
#include "Nucleus/Convert.h"

// Pedestal
#include "Pedestal/TEView.hh"


namespace UseEdit
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	namespace Ped = Pedestal;
	
	
	static Rect MakeWindowRect()
	{
		Rect screenBounds = N::GetQDGlobalsScreenBits().bounds;
		
		Rect rect = N::SetRect(0, 18, 2*4+6*80+16, 18+2*4+11*25+16);
		short mbarHeight = ::GetMBarHeight();
		short vMargin = (screenBounds.bottom - rect.bottom) - mbarHeight;
		short hMargin = (screenBounds.right - rect.right);
		
		return N::OffsetRect(rect, hMargin / 2, mbarHeight + vMargin / 3);
	}
	
	
	static inline std::auto_ptr< Ped::View > MakeView()
	{
		Rect scroller_bounds = MakeWindowRect();
		
		Rect subview_bounds = Pedestal::ScrollBounds< true, false >( scroller_bounds );
		
		View* scroller = NULL;
		
		std::auto_ptr< Ped::View > view( scroller = new View( scroller_bounds ) );
		
		std::auto_ptr< Ped::ScrollableBase > subview( new Ped::TEView( subview_bounds ) );
		
		scroller->SetSubView( subview );
		
		return view;
	}
	
	
	Window::Window( ConstStr255Param title )
	: Base( Ped::NewWindowContext( MakeWindowRect(), title ),
	  N::documentProc )
	{
		SetView( MakeView() );
	}
	
}

