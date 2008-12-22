/*	=========
 *	Window.hh
 *	=========
 */

#ifndef USEEDIT_WINDOW_HH
#define USEEDIT_WINDOW_HH

// Standard C++
#include <string>

// Pedestal
#include "Pedestal/Scroller.hh"
#include "Pedestal/Window.hh"


namespace UseEdit
{
	
	typedef Pedestal::Scroller< true > View;
	
	
	class Window : public Pedestal::Window
	{
		public:
			typedef Pedestal::Window Base;
			
			Window( ConstStr255Param title = "\p" "UseEdit" );
	};
	
}

#endif

