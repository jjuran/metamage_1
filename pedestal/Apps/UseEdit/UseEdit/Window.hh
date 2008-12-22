/*	=========
 *	Window.hh
 *	=========
 */

#ifndef USEEDIT_WINDOW_HH
#define USEEDIT_WINDOW_HH

// Pedestal
#include "Pedestal/Scroller.hh"
#include "Pedestal/Window.hh"


namespace UseEdit
{
	
	typedef Pedestal::Scroller< true > View;
	
	std::auto_ptr< Pedestal::Window >
	//
	NewWindow( ConstStr255Param title = "\p" "UseEdit" );
	
}

#endif

