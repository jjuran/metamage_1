/*	=========
 *	Window.hh
 *	=========
 */

#ifndef PEDESTAL_WINDOW_HH
#define PEDESTAL_WINDOW_HH

// Debug
#include "debug/boost_assert.hh"

// plus
#include "plus/ref_count.hh"

// Boost
#include <boost/intrusive_ptr.hpp>

// nucleus
#ifndef NUCLEUS_OWNED_HH
#include "nucleus/owned.hh"
#endif

// Nitrogen
#ifndef MAC_WINDOWS_TYPES_WINDOWREF_HH
#include "Mac/Windows/Types/WindowRef.hh"
#endif

// Pedestal
#ifndef PEDESTAL_VIEW_HH
#include "Pedestal/View.hh"
#endif


namespace Pedestal
{
	
	void ResizeWindow( WindowRef window, Point newSize );
	
	
	void SetWindowSize( WindowRef window, Point size );
	
	inline void SetWindowPosition( WindowRef window, Point position )
	{
		::MoveWindow( window, position.h, position.v, false );
	}
	
	
	class Window : public plus::ref_count< Window >
	{
		private:
			boost::intrusive_ptr< View > itsView;
			
			nucleus::owned< WindowRef > itsWindowRef;
		
		public:
			Window( nucleus::owned< WindowRef > window );
			
			~Window();
			
			WindowRef Get() const  { return itsWindowRef; }
			
			boost::intrusive_ptr< View >& GetView()  { return itsView; }
			
			void SetView( boost::intrusive_ptr< View > const& view )  { GetView() = view; }
	};
	
	View* get_window_view( WindowRef window );
	
	void window_activated( WindowRef window, bool activating );
	
	void window_mouseDown( WindowRef window, const EventRecord& event );
	
	void window_update( WindowRef window );
	
}

#endif
