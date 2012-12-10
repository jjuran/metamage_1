/*
	Window.hh
	---------
*/

#ifndef USEEDIT_WINDOW_HH
#define USEEDIT_WINDOW_HH

// Standard C++
#include <memory>

// plus
#include "plus/string_fwd.hh"

// Pedestal
#ifndef PEDESTAL_WINDOW_HH
#include "Pedestal/Window.hh"
#endif


namespace UseEdit
{
	
	class ScrollFrame;
	
	typedef ScrollFrame View;
	
	std::auto_ptr< Pedestal::Window >
	//
	NewWindow( ConstStr255Param title = "\p" "UseEdit" );
	
	void SetText( ScrollFrame& view, const plus::string& text );
	
}

#endif

