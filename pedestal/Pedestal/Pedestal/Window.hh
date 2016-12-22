/*	=========
 *	Window.hh
 *	=========
 */

#ifndef PEDESTAL_WINDOW_HH
#define PEDESTAL_WINDOW_HH

// plus
#include "plus/ref_count.hh"

// nucleus
#ifndef NUCLEUS_OWNED_HH
#include "nucleus/owned.hh"
#endif

// Nitrogen
#ifndef MAC_WINDOWS_TYPES_WINDOWREF_HH
#include "Mac/Windows/Types/WindowRef.hh"
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
			nucleus::owned< WindowRef > itsWindowRef;
		
		public:
			Window( nucleus::owned< WindowRef > window );
			
			~Window();
			
			WindowRef Get() const  { return itsWindowRef; }
	};
	
}

#endif
