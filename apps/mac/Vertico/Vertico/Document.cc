/*
	Vertico/Document.cc
	-------------------
*/

#include "Vertico/Document.hh"

// mac-qd-utils
#include "mac_qd/get_portRect.hh"
#include "mac_qd/globals/screenBits.hh"

// mac-app-utils
#include "mac_app/Window_menu.hh"

// debug
#include "debug/boost_assert.hh"

// Boost
#include <boost/intrusive_ptr.hpp>

// text-input
#include "text_input/feed.hh"
#include "text_input/get_line_from_feed.hh"

// Nitrogen
#include "Nitrogen/Files.hh"

// Pedestal
#include "Pedestal/WindowStorage.hh"

// worldview
#include "worldview/Parser.hh"

// Vertico
#include "Vertico/PortView.hh"


namespace Nitrogen
{
	
	class FSReader
	{
		private:
			FSFileRefNum its_fRefNum;
		
		public:
			typedef char*        first_argument_type;
			typedef std::size_t  second_argument_type;
			typedef std::size_t  result_type;
			
			FSReader( FSFileRefNum fRefNum ) : its_fRefNum( fRefNum )
			{
			}
			
			std::size_t operator()( char* buffer, std::size_t length ) const
			{
				return FSRead( its_fRefNum, length, buffer, ThrowEOF_Never() );
			}
	};
	
}

namespace Vertico
{
	
	namespace n = nucleus;
	namespace N = Nitrogen;
	namespace Ped = Pedestal;
	
	
	static inline
	bool has_GetAvailableWindowPositioningBounds()
	{
		/*
			Available in CarbonLib 1.3 and Mac OS X 10.0.
			
			g++ 3.1 warns about &GetAvailableWindowPositioningBounds
			always being true, so don't expose the code to it.
		*/
		
	#if ! TARGET_API_MAC_OSX
		
		return TARGET_API_MAC_CARBON  &&  &GetAvailableWindowPositioningBounds;
		
	#endif
		
		return true;
	}
	
	static Rect MakeWindowRect()
	{
		Rect rect;
		
		if ( has_GetAvailableWindowPositioningBounds() )
		{
			GetAvailableWindowPositioningBounds( GetMainDevice(), &rect );
		}
		else
		{
			rect = mac::qd::screenBits().bounds;
			
			rect.top += GetMBarHeight();
		}
		
		rect.top += 24;  // leave room for title bar
		
		UInt16 desktop_width  = rect.right - rect.left;
		UInt16 desktop_height = rect.bottom - rect.top;
		
		UInt16 max_width  = desktop_width - 12;
		UInt16 max_height = desktop_height - 8;
		
		UInt16 length = max_width < max_height ? max_width : max_height;
		
		if ( ! TARGET_API_MAC_CARBON  &&  length > 400 )
		{
			length = 400;
		}
		
		UInt16 left_margin = (desktop_width - length) / 2;
		
		rect.bottom = rect.top + length;
		
		rect.left  = left_margin;
		rect.right = left_margin + length;
		
		return rect;
	}
	
	static
	n::owned< WindowRef > CreateWindow( const Rect& bounds )
	{
		if ( ! TARGET_API_MAC_CARBON )
		{
			return Ped::CreateWindow( bounds, "\p" );
		}
		
		WindowAttributes attrs = kWindowCloseBoxAttribute
		                       | kWindowResizableAttribute
		                       | kWindowCollapseBoxAttribute
		                     #if CONFIG_COMPOSITING
		                       | kWindowCompositingAttribute
		                     #endif
		                     #ifdef MAC_OS_X_VERSION_10_3
		                       | kWindowAsyncDragAttribute
		                     #endif
		                     #ifdef MAC_OS_X_VERSION_10_7
		                       | kWindowHighResolutionCapableAttribute
		                     #endif
		                       ;
		
		return Ped::CreateWindow( kDocumentWindowClass, attrs, bounds );
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
		
		const Rect& bounds = mac::qd::get_portRect( window );
		
		boost::intrusive_ptr< Ped::View > view( new PortView( bounds ) );
		
		Ped::set_window_view( window, view.get() );
		
		LoadFileIntoWindow( file, window );
		
		ShowWindow( window );
		
		window.release();
	}
	
}
