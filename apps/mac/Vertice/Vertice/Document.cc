/*
	Vertice/Document.cc
	-------------------
*/

#include "Vertice/Document.hh"

// Standard C++
#include <algorithm>

// text-input
#include "text_input/feed.hh"
#include "text_input/get_line_from_feed.hh"

// Nitrogen
#include "Nitrogen/MacWindows.hh"

// Nitrogen Extras
#include "FSReader.hh"

// Pedestal
#include "Pedestal/Window.hh"
#include "Pedestal/WindowStorage.hh"

// Vertice
#include "Vertice/Parser.hh"
#include "Vertice/PortView.hh"


namespace Vertice
{
	
	namespace n = nucleus;
	namespace N = Nitrogen;
	namespace Ped = Pedestal;
	
	
	static Rect MakeWindowRect()
	{
		Rect screenBounds = N::GetQDGlobalsScreenBits().bounds;
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
		                          #ifdef MAC_OS_X_VERSION_10_2
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
	
	class Window : public Ped::Window
	{
		public:
			Window();
	};
	
	Window::Window()
	:
		Ped::Window( CreateWindow( MakeWindowRect() ) )
	{
		Rect bounds = N::GetPortBounds( N::GetWindowPort( Get() ) );
		
		boost::intrusive_ptr< Ped::View > view( new PortView( bounds ) );
		
		Ped::set_window_view( Get(), view.get() );
	}
	
	static
	void LoadFileIntoWindow( const FSSpec& file, WindowRef window )
	{
		using Ped::get_window_view;
		
		SetWTitle( window, file.name );
		
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
	
	static
	void DocumentClosed( WindowRef window )
	{
		Ped::Window* doc = Ped::get_window_owner( window );
		
		delete doc;
	}
	
	void OpenDocument( const FSSpec& file )
	{
		Window* doc = new Window();
		
		WindowRef window = doc->Get();
		
		Ped::set_window_closed_proc( window, &DocumentClosed );
		
		LoadFileIntoWindow( file, window );
		
		ShowWindow( window );
	}
	
}
