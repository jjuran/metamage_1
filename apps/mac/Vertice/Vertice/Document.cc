/*
	Vertice/Document.cc
	-------------------
*/

#include "Vertice/Document.hh"

// Standard C++
#include <algorithm>

// mac-qd-utils
#include "mac_qd/globals/screenBits.hh"

// mac-app-utils
#include "mac_app/Window_menu.hh"

// text-input
#include "text_input/feed.hh"
#include "text_input/get_line_from_feed.hh"

// Nitrogen
#include "Nitrogen/MacWindows.hh"

// Nitrogen Extras
#include "FSReader.hh"

// Pedestal
#include "Pedestal/WindowStorage.hh"

// worldview
#include "worldview/Parser.hh"

// Vertice
#include "Vertice/PortView.hh"


namespace Vertice
{
	
	namespace n = nucleus;
	namespace N = Nitrogen;
	namespace Ped = Pedestal;
	
	
	static Rect MakeWindowRect()
	{
		const Rect& screenBounds = mac::qd::screenBits().bounds;
		short mbarHeight = ::GetMBarHeight();
		Rect rect = screenBounds;
		rect.top += mbarHeight + 22;
		rect.bottom -= 7;
		rect.left = 6;
		rect.right -= 7;
		short length = std::min( rect.bottom - rect.top, 400 );
		rect.bottom = rect.top + length;
		//short vMargin = ( screenBounds.bottom - rect.bottom ) - mbarHeight;
		short hMargin = ( screenBounds.right - length );
		rect.left = hMargin / 2;
		rect.right = hMargin / 2 + length;
		
		return rect;
		return N::InsetRect( rect, 4, 4 );
	}
	
	static
	n::owned< WindowRef > CreateWindow( const Rect& bounds )
	{
		if ( ! TARGET_API_MAC_CARBON )
		{
			return Ped::CreateWindow( bounds, "\p" );
		}
		
		Mac::WindowAttributes attrs = Mac::kWindowCloseBoxAttribute
		                            | Mac::kWindowResizableAttribute
		                            | Mac::kWindowCollapseBoxAttribute
		                          #ifdef MAC_OS_X_VERSION_10_3  // not 10.2
		                            | Mac::kWindowCompositingAttribute
		                          #endif
		                          #ifdef MAC_OS_X_VERSION_10_3
		                            | Mac::kWindowAsyncDragAttribute
		                          #endif
		                          #ifdef MAC_OS_X_VERSION_10_7
		                            | Mac::kWindowHighResolutionCapableAttribute
		                          #endif
		                            ;
		
		return Ped::CreateWindow( Mac::kDocumentWindowClass, attrs, bounds );
	}
	
	static
	void LoadFileIntoWindow( const FSSpec& file, WindowRef window )
	{
		using Ped::get_window_view;
		
		SetWTitle( window, file.name );
		
		mac::app::Window_menu_remove( window );
		mac::app::Window_menu_insert( window );
		
		n::owned< N::FSFileRefNum > fRefNum = N::FSpOpenDF( file, N::fsRdPerm );
		
		PortView& view = static_cast< PortView& >( *get_window_view( window ) );
		
		Loader loader( view.ItsScene() );
		
		text_input::feed feed;
		
		N::FSReader reader( fRefNum );
		
		while ( const plus::string* s = get_line_bare_from_feed( feed, reader ) )
		{
			const plus::string& line = *s;
			
			loader.LoadLine( line );
		}
		
		view.Render();
	}
	
	void OpenDocument( const FSSpec& file )
	{
		n::owned< WindowRef > window = CreateWindow( MakeWindowRect() );
		
		Rect bounds = N::GetPortBounds( N::GetWindowPort( window ) );
		
		boost::intrusive_ptr< Ped::View > view( new PortView( bounds ) );
		
		Ped::set_window_view( window, view.get() );
		
		LoadFileIntoWindow( file, window );
		
		ShowWindow( window );
		
		window.release();
	}
	
}
