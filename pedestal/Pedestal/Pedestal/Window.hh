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
#ifndef MAC_WINDOWS_TYPES_WINDOWDEFPROCID_HH
#include "Mac/Windows/Types/WindowDefProcID.hh"
#endif
#ifndef MAC_WINDOWS_TYPES_WINDOWREF_HH
#include "Mac/Windows/Types/WindowRef.hh"
#endif

// Pedestal
#ifndef PEDESTAL_VIEW_HH
#include "Pedestal/View.hh"
#endif


namespace Pedestal
{
	
	typedef void (*WindowClosed_proc )( WindowRef window );
	typedef void (*WindowResized_proc)( WindowRef window );
	
	
	void ResizeWindow( WindowRef window, Point newSize );
	
	
	void SetWindowSize( WindowRef window, Point size );
	
	inline void SetWindowPosition( WindowRef window, Point position )
	{
		::MoveWindow( window, position.h, position.v, false );
	}
	
	struct NewWindowContext
	{
		const Rect&           bounds;
		ConstStr255Param      title;
		bool                  visible;
		Mac::WindowDefProcID  procID;
		bool                  goAwayFlag;
		
		NewWindowContext( const Rect&           bounds,
			              ConstStr255Param      title,
			              bool                  visible    = true,
			              Mac::WindowDefProcID  procID     = Mac::documentProc,
			              bool                  goAwayFlag = true )
		:
			bounds    ( bounds     ),
			title     ( title      ),
			visible   ( visible    ),
			procID    ( procID     ),
			goAwayFlag( goAwayFlag )
		{
		}
	};
	
	
	class Window : public plus::ref_count< Window >
	{
		private:
			boost::intrusive_ptr< View > itsView;
			
			nucleus::owned< WindowRef > itsWindowRef;
		
		public:
			Window( const NewWindowContext& context );
			
			Window( nucleus::owned< WindowRef > window );
			
			~Window();
			
			WindowRef Get() const  { return itsWindowRef; }
			
			boost::intrusive_ptr< View >& GetView()  { return itsView; }
			
			void SetView( boost::intrusive_ptr< View > const& view )  { GetView() = view; }
	};
	
	WindowAttributes get_window_attributes( WindowRef window );
	
	Window* get_window_owner( WindowRef window );
	
	View* get_window_view( WindowRef window );
	
	void window_activated( WindowRef window, bool activating );
	
	void window_mouseDown( WindowRef window, const EventRecord& event );
	
	void window_update( WindowRef window );
	
	void set_window_closed_proc( WindowRef          window,
	                             WindowClosed_proc  proc );
	
	void set_window_resized_proc( WindowRef           window,
	                              WindowResized_proc  proc );
	
	void close_window( WindowRef window );
	
}

#endif
