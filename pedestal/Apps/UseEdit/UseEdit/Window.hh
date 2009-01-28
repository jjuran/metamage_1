/*	=========
 *	Window.hh
 *	=========
 */

#ifndef USEEDIT_WINDOW_HH
#define USEEDIT_WINDOW_HH

// Pedestal
#include "Pedestal/Window.hh"


namespace UseEdit
{
	
	std::auto_ptr< Pedestal::Window >
	//
	NewWindow( ConstStr255Param title = "\p" "UseEdit" );
	
}

#endif

